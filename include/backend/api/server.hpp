#pragma once

#include <string>
#include <memory>
#include <backend/dal/idal_product.hpp>
#include <backend/business_logic/product_service.hpp>
#include <backend/business_logic/inventory_service.hpp>
#include <backend/business_logic/checkout_service.hpp>
// Forward declarations or include crow?
// Usually, we can just forward-declare but Crow uses heavy templates.
// It is better to include crow minimal or just include it.
#include <crow.h>

namespace hero::api {

/**
 * @class Server
 * @brief RESTful API Server using Crow framework
 * 
 * Implements routes for:
 * - Product CRUD operations
 * - Category management
 * - Supplier management
 * - Checkout/Transactions
 * - Inventory management
 * 
 * Middleware integration:
 * - JWT Authentication
 * - Request Logging
 */
class Server {
public:
    Server() = default;
    ~Server() = default;

    // Server lifecycle
    bool initialize(crow::SimpleApp& app, int port = 8080);
    void run();
    void stop();
    bool isRunning() const { return running_; }

    // Configuration
    void setPort(int port) { port_ = port; }
    void setHost(const std::string& host) { host_ = host; }
    int getPort() const { return port_; }

private:
    int port_ = 8080;
    std::string host_ = "0.0.0.0";
    bool running_ = false;

    std::shared_ptr<dal::IDalProduct> dal_product_;
    std::unique_ptr<business_logic::ProductService> product_service_;
    std::unique_ptr<business_logic::InventoryService> inventory_service_;
    std::unique_ptr<business_logic::CheckoutService> checkout_service_;

    // Route handlers
    void setupProductRoutes();
    void setupCategoryRoutes();
    void setupSupplierRoutes();
    void setupCheckoutRoutes();
    void setupInventoryRoutes();
    void setupAuthRoutes();

    // Middleware setup
    void setupMiddleware();

    crow::SimpleApp* app_ = nullptr;
};

} // namespace hero::api
