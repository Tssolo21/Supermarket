#include <backend/business_logic/checkout_service.hpp>
#include <backend/business_logic/product_service.hpp>
#include <backend/business_logic/inventory_service.hpp>
#include <iostream>
#include <chrono>

namespace hero::business_logic {

using Transaction = hero::models::Transaction;
using Product = hero::models::Product;
using ProductId = Product::ProductId;

CheckoutService::CheckoutService(std::shared_ptr<dal::IDalProduct> dal,
                                 std::shared_ptr<ProductService> product_service,
                                 std::shared_ptr<InventoryService> inventory_service)
    : dal_(std::move(dal)), 
      product_service_(std::move(product_service)), 
      inventory_service_(std::move(inventory_service)) {}

CheckoutService::CheckoutResult CheckoutService::processCheckout(
    const std::vector<std::pair<ProductId, int>>& items,
    const std::string& cashier,
    Transaction::PaymentMethod method) {
    
    validateCheckoutItems(items);
    
    // Validate stock availability using injected inventory service
    for (const auto& [product_id, quantity] : items) {
        if (!inventory_service_->hasEnoughStock(product_id, quantity)) {
            return CheckoutResult{false, -1, 0.0, "Insufficient stock for product " + std::to_string(product_id)};
        }
    }
    
    // Create transaction
    auto transaction = createTransaction(items, cashier);
    transaction.setPaymentMethod(method);
    transaction.finalize();
    
    // Decrement stock using injected product service
    for (const auto& [product_id, quantity] : items) {
        if (!product_service_->sellProduct(product_id, quantity)) {
            return CheckoutResult{false, -1, 0.0, "Failed to update stock for product"};
        }
    }
    
    // TODO: Persist transaction using dal_ once IDalTransaction is ready
    
    return CheckoutResult{true, transaction.getId(), transaction.getTotalAmount(), "Checkout successful"};
}

std::future<CheckoutService::CheckoutResult> CheckoutService::processCheckoutAsync(
    const std::vector<std::pair<ProductId, int>>& items,
    const std::string& cashier,
    Transaction::PaymentMethod method) {
    
    return std::async(std::launch::async, [this, items, cashier, method]() {
        return this->processCheckout(items, cashier, method);
    });
}

std::optional<Transaction> CheckoutService::getTransaction(TransactionId id) {
    // TODO: Implement transaction retrieval from DAL
    return std::nullopt;
}

std::vector<Transaction> CheckoutService::getTransactionsByDateRange(time_t start, time_t end) {
    // TODO: Implement date range query
    return {};
}

std::vector<Transaction> CheckoutService::getTransactionsByCashier(const std::string& cashier) {
    // TODO: Implement cashier-based query
    return {};
}

double CheckoutService::getDailySales(time_t date) {
    // TODO: Implement daily sales calculation
    return 0.0;
}

int CheckoutService::getDailyTransactionCount(time_t date) {
    // TODO: Implement transaction count
    return 0;
}

void CheckoutService::validateCheckoutItems(const std::vector<std::pair<ProductId, int>>& items) {
    if (items.empty()) {
        throw std::invalid_argument("Checkout items cannot be empty");
    }
    
    for (const auto& [product_id, quantity] : items) {
        if (product_id < 0) {
            throw std::invalid_argument("Invalid product ID");
        }
        if (quantity <= 0) {
            throw std::invalid_argument("Quantity must be positive");
        }
    }
}

hero::models::Transaction CheckoutService::createTransaction(const std::vector<std::pair<ProductId, int>>& items,
                                               const std::string& cashier) {
    Transaction transaction(0);
    transaction.setCashier(cashier);
    
    for (const auto& [product_id, quantity] : items) {
        auto product = product_service_->getProduct(product_id);
        if (!product) continue;
        
        Transaction::TransactionItem item{
            product_id,
            product->getName(),
            quantity,
            product->getPrice(),
            product->getPrice() * quantity
        };
        
        transaction.addItem(item);
    }
    
    return transaction;
}

} // namespace hero::business_logic
