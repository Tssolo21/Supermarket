#include <backend/api/server.hpp>
#include <backend/dal/database.hpp>
#include <backend/dal/dal_product.hpp>
#include <backend/business_logic/product_service.hpp>
#include <backend/business_logic/checkout_service.hpp>
#include <backend/business_logic/inventory_service.hpp>
#include <backend/dal/dal_category.hpp>
#include <backend/dal/dal_supplier.hpp>
#include <backend/middleware/jwt_authenticator.hpp>
#include <backend/middleware/request_logger.hpp>
#include <backend/concurrency/async_task_scheduler.hpp>
#include <nlohmann/json.hpp>
#include <iostream>

namespace hero::api {

bool Server::initialize(crow::SimpleApp& app, int port) {
    app_ = &app;
    port_ = port;
    
    // Initialize database
    auto& db = dal::Database::getInstance();
    if (!db.initialize("supermarket.db")) {
        std::cerr << "Failed to initialize database" << std::endl;
        return false;
    }

    // Instantiate Dependencies (Dependency Injection)
    dal_product_ = std::make_shared<dal::DalProduct>();
    auto dal_transaction = std::make_shared<dal::DalTransaction>();
    product_service_ = std::make_unique<business_logic::ProductService>(dal_product_);
    inventory_service_ = std::make_unique<business_logic::InventoryService>(dal_product_);
    checkout_service_ = std::make_unique<business_logic::CheckoutService>(dal_product_,
        dal_transaction,
        std::shared_ptr<business_logic::ProductService>(product_service_.get(), [](auto*){}), // Non-owning shared_ptr
        std::shared_ptr<business_logic::InventoryService>(inventory_service_.get(), [](auto*){}));

    // Initialize async scheduler
    concurrency::AsyncTaskScheduler::getInstance().initialize(4);

    // Initialize request logger
    middleware::RequestLogger::getInstance().setLogFile("requests.log");
    middleware::RequestLogger::getInstance().enableConsoleOutput(true);

    // Setup routes
    setupProductRoutes();
    setupCategoryRoutes();
    setupSupplierRoutes();
    setupCheckoutRoutes();
    setupInventoryRoutes();
    setupAuthRoutes();
    setupMiddleware();

    running_ = true;
    std::cout << "[Server] Initialized on " << host_ << ":" << port_ << std::endl;
    return true;
}

void Server::run() {
    if (!running_) return;
    std::cout << "[Server] Starting server on " << host_ << ":" << port_ << "..." << std::endl;
    app_->port(port_).multithreaded().run();
}

void Server::stop() {
    running_ = false;
    app_->stop();
    concurrency::AsyncTaskScheduler::getInstance().shutdown();
    std::cout << "[Server] Server stopped" << std::endl;
}

// Helper for CORS headers
inline crow::response cors(crow::response&& res) {
    res.add_header("Access-Control-Allow-Origin", "*");
    res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
    return res;
}

void Server::setupProductRoutes() {
    std::cout << "[Server] Setting up product routes..." << std::endl;
    auto& a = *app_;

    CROW_ROUTE(a, "/api/products")
    .methods(crow::HTTPMethod::GET)(
        [this]() {
            auto products = product_service_->getAllProducts();
            nlohmann::json arr = nlohmann::json::array();
            for (const auto& p : products) {
                arr.push_back(p.toJson());
            }
            return cors(crow::response(200, arr.dump()));
        });

    CROW_ROUTE(a, "/api/products")
    .methods(crow::HTTPMethod::POST)(
        [this](const crow::request& req) {
            auto body = nlohmann::json::parse(req.body, nullptr, false);
            if (body.is_discarded()) {
                return crow::response(400, "Invalid JSON");
            }
            try {
                auto product = models::Product::fromJson(body);
                auto id = product_service_->addProduct(product);
                nlohmann::json result;
                result["id"] = id;
                return cors(crow::response(201, result.dump()));
            } catch (const std::exception& e) {
                return cors(crow::response(400, e.what()));
            }
        });

    CROW_ROUTE(a, "/api/products/<int>")
    .methods(crow::HTTPMethod::GET)(
        [this](int id) {
            auto product = product_service_->getProduct(id);
            if (product) {
                return cors(crow::response(200, product->toJson().dump()));
            }
            return cors(crow::response(404, "Product not found"));
        });

    CROW_ROUTE(a, "/api/products/<int>")
    .methods(crow::HTTPMethod::PUT)(
        [this](const crow::request& req, int id) {
            auto body = nlohmann::json::parse(req.body, nullptr, false);
            if (body.is_discarded()) return crow::response(400, "Invalid JSON");
            try {
                auto product = models::Product::fromJson(body);
                models::Product p(id, product.getName(), product.getSku(),
                                  product.getPrice(), product.getStock(),
                                  product.getCategoryId(), product.getSupplierId());
                if (product_service_->updateProduct(p)) {
                    return crow::response(200, "OK");
                }
                return crow::response(404, "Product not found");
            } catch (const std::exception& e) {
                return cors(crow::response(400, e.what()));
            }
        });

    CROW_ROUTE(a, "/api/products/<int>")
    .methods(crow::HTTPMethod::Delete)(
        [this](int id) {
            if (product_service_->deleteProduct(id)) {
                return cors(crow::response(200, "OK"));
            }
            return cors(crow::response(404, "Product not found"));
        });

    CROW_ROUTE(a, "/api/products/search")
    .methods(crow::HTTPMethod::GET)(
        [this](const crow::request& req) {
            std::string query;
            if (req.url_params.get("q")) {
                query = req.url_params.get("q");
            }
            auto products = product_service_->searchProducts(query);
            nlohmann::json arr = nlohmann::json::array();
            for (const auto& p : products) {
                arr.push_back(p.toJson());
            }
            return cors(crow::response(200, arr.dump()));
        });
}

void Server::setupCategoryRoutes() {
    std::cout << "[Server] Setting up category routes..." << std::endl;
    auto& a = *app_;

    CROW_ROUTE(a, "/api/categories")
    .methods(crow::HTTPMethod::GET)(
        []() {
            dal::DalCategory dal;
            auto categories = dal.getAll();
            nlohmann::json arr = nlohmann::json::array();
            for (const auto& c : categories) {
                arr.push_back(c.toJson());
            }
            return cors(crow::response(200, arr.dump()));
        });

    CROW_ROUTE(a, "/api/categories")
    .methods(crow::HTTPMethod::POST)(
        [](const crow::request& req) {
            auto body = nlohmann::json::parse(req.body, nullptr, false);
            if (body.is_discarded()) return crow::response(400, "Invalid JSON");
            dal::DalCategory dal;
            auto category = models::Category::fromJson(body);
            auto id = dal.create(category);
            nlohmann::json result;
            result["id"] = id;
            return cors(crow::response(201, result.dump()));
        });
}

void Server::setupSupplierRoutes() {
    std::cout << "[Server] Setting up supplier routes..." << std::endl;
    auto& a = *app_;

    CROW_ROUTE(a, "/api/suppliers")
    .methods(crow::HTTPMethod::GET)(
        []() {
            dal::DalSupplier dal;
            auto suppliers = dal.getAll();
            nlohmann::json arr = nlohmann::json::array();
            for (const auto& s : suppliers) {
                arr.push_back(s.toJson());
            }
            return cors(crow::response(200, arr.dump()));
        });

    CROW_ROUTE(a, "/api/suppliers")
    .methods(crow::HTTPMethod::POST)(
        [](const crow::request& req) {
            auto body = nlohmann::json::parse(req.body, nullptr, false);
            if (body.is_discarded()) return crow::response(400, "Invalid JSON");
            dal::DalSupplier dal;
            auto supplier = models::Supplier::fromJson(body);
            auto id = dal.create(supplier);
            nlohmann::json result;
            result["id"] = id;
            return crow::response(201, result.dump());
        });
}

void Server::setupCheckoutRoutes() {
    std::cout << "[Server] Setting up checkout routes..." << std::endl;
    auto& a = *app_;

    CROW_ROUTE(a, "/api/checkout")
    .methods(crow::HTTPMethod::POST)(
        [this](const crow::request& req) {
            auto body = nlohmann::json::parse(req.body, nullptr, false);
            if (body.is_discarded() || !body.contains("items") || !body.contains("cashier")) {
                return cors(crow::response(400, "Invalid JSON or missing fields"));
            }
            try {
                std::vector<std::pair<models::Product::ProductId, int>> items;
                for (const auto& item : body["items"]) {
                    items.push_back({item["product_id"].get<int64_t>(),
                                     item["quantity"].get<int>()});
                }

                auto method = models::Transaction::PaymentMethod::CASH;
                if (body.contains("payment_method")) {
                    std::string m = body["payment_method"].get<std::string>();
                    if (m == "CARD") method = models::Transaction::PaymentMethod::CARD;
                    else if (m == "MOBILE_PAYMENT") method = models::Transaction::PaymentMethod::MOBILE_PAYMENT;
                    else if (m == "CHECK") method = models::Transaction::PaymentMethod::CHECK;
                }

                auto result = checkout_service_->processCheckout(items, body["cashier"].get<std::string>(), method);

                nlohmann::json res_json;
                res_json["success"] = result.success;
                res_json["transaction_id"] = result.transaction_id;
                res_json["total_amount"] = result.total_amount;
                res_json["message"] = result.message;

                return cors(crow::response(result.success ? 200 : 400, res_json.dump()));
            } catch (const std::exception& e) {
                return cors(crow::response(400, e.what()));
            }
        });
}

void Server::setupInventoryRoutes() {
    std::cout << "[Server] Setting up inventory routes..." << std::endl;
    auto& a = *app_;

    CROW_ROUTE(a, "/api/inventory/stats")
    .methods(crow::HTTPMethod::GET)(
        [this]() {
            auto stats = inventory_service_->getInventoryStats();
            nlohmann::json out;
            out["total_products"] = stats.total_products;
            out["low_stock_items"] = stats.low_stock_items;
            out["out_of_stock_items"] = stats.out_of_stock_items;
            out["total_inventory_value"] = stats.total_inventory_value;
            out["average_price"] = stats.average_price;
            return cors(crow::response(200, out.dump()));
        });

    CROW_ROUTE(a, "/api/inventory/low-stock")
    .methods(crow::HTTPMethod::GET)(
        [this](const crow::request& req) {
            int threshold = 10;
            if (req.url_params.get("threshold")) {
                threshold = std::stoi(req.url_params.get("threshold"));
            }
            auto products = inventory_service_->getLowStockProducts(threshold);
            nlohmann::json arr = nlohmann::json::array();
            for (const auto& p : products) {
                arr.push_back(p.toJson());
            }
            return cors(crow::response(200, arr.dump()));
        });

    CROW_ROUTE(a, "/api/inventory/out-of-stock")
    .methods(crow::HTTPMethod::GET)(
        [this]() {
            auto products = inventory_service_->getOutOfStockProducts();
            nlohmann::json arr = nlohmann::json::array();
            for (const auto& p : products) {
                arr.push_back(p.toJson());
            }
            return cors(crow::response(200, arr.dump()));
        });
}

void Server::setupAuthRoutes() {
    std::cout << "[Server] Setting up auth routes..." << std::endl;
    auto& a = *app_;

    CROW_ROUTE(a, "/api/auth/login")
    .methods(crow::HTTPMethod::POST)(
        [](const crow::request& req) {
            auto body = nlohmann::json::parse(req.body, nullptr, false);
            if (body.is_discarded() || !body.contains("username") || !body.contains("password")) {
                return cors(crow::response(400, "Missing credentials"));
            }
            std::string username = body["username"].get<std::string>();
            std::string password = body["password"].get<std::string>();

            if (username == "admin" && password == "admin") {
                middleware::JwtAuthenticator::TokenPayload payload;
                payload.user_id = "1";
                payload.username = "admin";
                payload.role = "ADMIN";
                payload.email = "admin@supermarket.com";
                auto token = middleware::JwtAuthenticator::getInstance().generateToken(payload);
                nlohmann::json result;
                result["token"] = token;
                return cors(crow::response(200, result.dump()));
            }
            return cors(crow::response(401, "Invalid credentials"));
        });
}

void Server::setupMiddleware() {
    std::cout << "[Server] Setting up middleware..." << std::endl;
    auto& a = *app_;

    // Handle OPTIONS for CORS preflight
    CROW_ROUTE(a, "/api/<path>")
    .methods(crow::HTTPMethod::OPTIONS)(
        [](std::string path) {
            return cors(crow::response(204));
        });
}

} // namespace hero::api
