#include <iostream>
#include <cassert>
#include <backend/models/product.hpp>
#include <backend/models/category.hpp>
#include <backend/models/supplier.hpp>

void test_product_creation() {
    hero::models::Product p(1, "Laptop", "SKU-001", 999.99, 10, 1, 1);
    assert(p.getId() == 1);
    assert(p.getName() == "Laptop");
    assert(p.getPrice() == 999.99);
    std::cout << "✓ Product creation test passed" << std::endl;
}

void test_product_json_serialization() {
    hero::models::Product p(1, "Keyboard", "SKU-002", 79.99, 50, 1, 1);
    auto json = p.toJson();
    
    auto p2 = hero::models::Product::fromJson(json);
    assert(p2.getId() == p.getId());
    assert(p2.getName() == p.getName());
    std::cout << "✓ Product JSON serialization test passed" << std::endl;
}

void test_stock_management() {
    hero::models::Product p(1, "Mouse", "SKU-003", 29.99, 100, 1, 1);
    
    assert(p.addStock(50) == true);
    assert(p.getStock() == 150);
    
    assert(p.removeStock(30) == true);
    assert(p.getStock() == 120);
    
    assert(p.removeStock(200) == false); // Should fail
    std::cout << "✓ Stock management test passed" << std::endl;
}

void test_category_creation() {
    hero::models::Category c(1, "Electronics", "Electronic devices");
    assert(c.getId() == 1);
    assert(c.getName() == "Electronics");
    std::cout << "✓ Category creation test passed" << std::endl;
}

void test_supplier_creation() {
    hero::models::Supplier s(1, "TechCorp", "contact@techcorp.com", "+1234567890", "123 Tech St");
    assert(s.getId() == 1);
    assert(s.getName() == "TechCorp");
    assert(s.getEmail() == "contact@techcorp.com");
    std::cout << "✓ Supplier creation test passed" << std::endl;
}

int test_models_main() {
    std::cout << "\n===== Model Tests =====" << std::endl;
    
    try {
        test_product_creation();
        test_product_json_serialization();
        test_stock_management();
        test_category_creation();
        test_supplier_creation();
        
        std::cout << "\n✓ All model tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
}
