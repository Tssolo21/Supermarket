#include <iostream>
#include <cassert>
#include <backend/dal/database.hpp>
#include <backend/dal/dal_product.hpp>
#include <backend/dal/dal_category.hpp>
#include <backend/dal/dal_supplier.hpp>
#include <backend/dal/dal_transaction.hpp>
#include <backend/models/product.hpp>
#include <backend/models/transaction.hpp>

using namespace hero::dal;
using namespace hero::models;

void test_dal_product_create_and_retrieve() {
    std::cout << "Testing DAL Product create and retrieve..." << std::endl;
    auto& db = Database::getInstance();
    db.initialize("test_supermarket.db");

    DalCategory catDal;
    DalSupplier supDal;
    catDal.getAll(); // Triggers table creation
    supDal.getAll(); // Triggers table creation

    db.execute("INSERT OR IGNORE INTO categories (id, name) VALUES (1, 'Test Category')");
    db.execute("INSERT OR IGNORE INTO suppliers (id, name) VALUES (1, 'Test Supplier')");

    DalProduct dal;
    Product p(0, "Test Product DAL", "DAL-SKU-1", 10.50, 100, 1, 1);

    auto id = dal.create(p);
    assert(id != -1);

    auto retrieved = dal.getById(id);
    assert(retrieved.has_value());
    assert(retrieved->getName() == "Test Product DAL");
    assert(retrieved->getSku() == "DAL-SKU-1");

    std::cout << "✓ DAL Product create and retrieve passed!" << std::endl;
}

void test_dal_transaction_create_and_retrieve() {
    std::cout << "Testing DAL Transaction create and retrieve..." << std::endl;
    DalTransaction dal;
    Transaction tx(0);
    tx.setCashier("Test Cashier");
    tx.setPaymentMethod(Transaction::PaymentMethod::CARD);

    Transaction::TransactionItem item{1, "Test Item", 2, 5.0, 10.0};
    tx.addItem(item);
    tx.finalize(); // Note: finalize() adds 10% tax

    auto id = dal.create(tx);
    assert(id != -1);

    auto retrieved = dal.getById(id);
    assert(retrieved.has_value());
    assert(retrieved->getCashier() == "Test Cashier");
    assert(retrieved->getTotalAmount() == 11.0); // 10.0 + 1.0 tax
    assert(retrieved->getItems().size() == 1);
    assert(retrieved->getItems()[0].product_name == "Test Item");

    std::cout << "✓ DAL Transaction create and retrieve passed!" << std::endl;
}

int test_dal_main() {
    std::cout << "\n===== DAL Tests =====" << std::endl;
    
    try {
        test_dal_product_create_and_retrieve();
        test_dal_transaction_create_and_retrieve();
        std::cout << "✓ All DAL tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "DAL Test failed: " << e.what() << std::endl;
        return 1;
    }
}
