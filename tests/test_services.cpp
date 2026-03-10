#include <iostream>
#include <cassert>
#include <memory>
#include <backend/business_logic/product_service.hpp>
#include <backend/business_logic/inventory_service.hpp>
#include <backend/business_logic/checkout_service.hpp>
#include <backend/dal/dal_product.hpp>
#include <backend/dal/dal_transaction.hpp>
#include <backend/dal/database.hpp>

using namespace hero::business_logic;
using namespace hero::dal;
using namespace hero::models;

void test_checkout_service_flow() {
    std::cout << "Testing Checkout Service Flow..." << std::endl;
    auto& db = Database::getInstance();
    db.initialize("test_supermarket.db");

    auto dal_product = std::make_shared<DalProduct>();
    auto dal_transaction = std::make_shared<DalTransaction>();

    auto product_service = std::make_shared<ProductService>(dal_product);
    auto inventory_service = std::make_shared<InventoryService>(dal_product);
    CheckoutService checkout_service(dal_product, dal_transaction, product_service, inventory_service);

    // Add a product first
    Product p(0, "Checkout Test Product", "CHK-SKU-1", 20.0, 50, 1, 1);
    auto pid = product_service->addProduct(p);
    assert(pid != -1);

    // Process checkout
    std::vector<std::pair<Product::ProductId, int>> items = {{pid, 2}};
    auto result = checkout_service.processCheckout(items, "Cashier 1", Transaction::PaymentMethod::CASH);

    assert(result.success);
    assert(result.total_amount == 44.0); // 40.0 + 10% tax
    assert(result.transaction_id != -1);

    // Verify stock decreased
    auto updated_product = product_service->getProduct(pid);
    assert(updated_product->getStock() == 48);

    // Verify transaction persisted
    auto tx = checkout_service.getTransaction(result.transaction_id);
    assert(tx.has_value());
    assert(tx->getTotalAmount() == 44.0);

    std::cout << "✓ Checkout service flow passed!" << std::endl;
}

void test_inventory_analytics() {
    std::cout << "Testing Inventory Analytics..." << std::endl;
    auto dal_product = std::make_shared<DalProduct>();
    InventoryService inventory_service(dal_product);

    auto stats = inventory_service.getInventoryStats();
    assert(stats.total_products > 0);

    std::cout << "✓ Inventory analytics passed!" << std::endl;
}

int test_services_main() {
    std::cout << "\n===== Business Logic Tests =====" << std::endl;
    
    try {
        test_checkout_service_flow();
        test_inventory_analytics();
        std::cout << "✓ All service tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Service Test failed: " << e.what() << std::endl;
        return 1;
    }
}
