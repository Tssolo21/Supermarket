#include <iostream>
#include <cassert>
#include <memory>
#include "backend/business_logic/product_service.hpp"
#include "backend/business_logic/alert_system.hpp"
#include "backend/dal/database.hpp"
#include "backend/dal/dal_product.hpp"
#include "backend/dal/dal_category.hpp"
#include "backend/dal/dal_supplier.hpp"

using namespace hero::business_logic;
using namespace hero::models;

class MockObserver : public hero::core::Observer<InventoryEvent> {
public:
    int callCount = 0;
    InventoryEvent lastEvent;

    void onNotify(const InventoryEvent& event) override {
        callCount++;
        lastEvent = event;
    }
};

void test_observer_integration() {
    std::cout << "Testing Observer Integration..." << std::endl;

    auto& db = hero::dal::Database::getInstance();
    db.initialize("test_supermarket.db");

    // Explicitly create tables to avoid dependency issues in tests
    db.execute("CREATE TABLE IF NOT EXISTS categories (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT NOT NULL UNIQUE, description TEXT, created_at INTEGER)");
    db.execute("CREATE TABLE IF NOT EXISTS suppliers (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT NOT NULL, email TEXT, phone TEXT, address TEXT, created_at INTEGER)");
    db.execute(R"(
        CREATE TABLE IF NOT EXISTS products (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            sku TEXT NOT NULL UNIQUE,
            buy_price REAL NOT NULL DEFAULT 0.0,
            price REAL NOT NULL,
            margin REAL NOT NULL DEFAULT 0.0,
            barcode TEXT,
            expiration_date INTEGER DEFAULT 0,
            stock INTEGER NOT NULL DEFAULT 0,
            category_id INTEGER,
            supplier_id INTEGER,
            description TEXT,
            created_at INTEGER,
            updated_at INTEGER,
            FOREIGN KEY(category_id) REFERENCES categories(id),
            FOREIGN KEY(supplier_id) REFERENCES suppliers(id)
        )
    )");

    // Initialize all tables to satisfy foreign keys
    hero::dal::DalCategory catDal;
    hero::dal::DalSupplier supDal;
    hero::dal::DalProduct prodDal;
    
    // Clean up or ensure a product exists
    auto existing = prodDal.getBySku("TEST-OBS-1");
    if (existing) {
        prodDal.deleteById(existing->getId());
    }
    
    // Ensure category 1 and supplier 1 exist for foreign keys
    db.execute("INSERT OR IGNORE INTO categories (id, name) VALUES (1, 'Test Category')");
    db.execute("INSERT OR IGNORE INTO suppliers (id, name) VALUES (1, 'Test Supplier')");

    Product p(0, "Test Product", "TEST-OBS-1", 10.0, 50, 1, 1);
    auto productId = prodDal.create(p);
    assert(productId != -1);
    
    auto product_dal_ptr = std::make_shared<hero::dal::DalProduct>();
    ProductService productService(product_dal_ptr);
    auto& notifier = InventoryNotifier::getInstance();
    auto mock = std::make_shared<MockObserver>();
    notifier.addObserver(mock);

    std::cout << "Selling product to trigger notification..." << std::endl;
    bool success = productService.sellProduct(productId, 5);
    assert(success);

    assert(mock->callCount == 1);
    assert(mock->lastEvent.type == InventoryEvent::Type::STOCK_UPDATED);
    assert(mock->lastEvent.product.getSku() == "TEST-OBS-1");
    assert(mock->lastEvent.product.getStock() == 45);

    std::cout << "✓ Observer integration test passed!" << std::endl;
    
    notifier.removeObserver(mock);
}

int test_observer_main() {
    try {
        test_observer_integration();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
