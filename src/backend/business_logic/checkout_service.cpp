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
                                 std::shared_ptr<dal::DalTransaction> transaction_dal,
                                 std::shared_ptr<ProductService> product_service,
                                 std::shared_ptr<InventoryService> inventory_service)
    : dal_(std::move(dal)), 
      transaction_dal_(std::move(transaction_dal)),
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
    
    // Persist transaction
    if (transaction_dal_) {
        auto id = transaction_dal_->create(transaction);
        if (id != -1) {
            return CheckoutResult{true, id, transaction.getTotalAmount(), "Checkout successful"};
        } else {
            return CheckoutResult{false, -1, 0.0, "Failed to persist transaction"};
        }
    }
    
    return CheckoutResult{true, transaction.getId(), transaction.getTotalAmount(), "Checkout successful (not persisted)"};
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
    if (!transaction_dal_) return std::nullopt;
    return transaction_dal_->getById(id);
}

std::vector<Transaction> CheckoutService::getTransactionsByDateRange(time_t start, time_t end) {
    if (!transaction_dal_) return {};
    return transaction_dal_->getBetweenDates(static_cast<int64_t>(start), static_cast<int64_t>(end));
}

std::vector<Transaction> CheckoutService::getTransactionsByCashier(const std::string& cashier) {
    if (!transaction_dal_) return {};
    // Filter transactions by cashier from a broad date range (or implement specific DAL method)
    auto txs = transaction_dal_->getBetweenDates(0, 2147483647); // Far future
    std::vector<Transaction> filtered;
    for (const auto& tx : txs) {
        if (tx.getCashier() == cashier) {
            filtered.push_back(tx);
        }
    }
    return filtered;
}

double CheckoutService::getDailySales(time_t date) {
    if (!transaction_dal_) return 0.0;

    struct tm* timeinfo = std::localtime(&date);
    timeinfo->tm_hour = 0;
    timeinfo->tm_min = 0;
    timeinfo->tm_sec = 0;
    time_t start = std::mktime(timeinfo);
    time_t end = start + 86400; // 24 hours

    auto txs = transaction_dal_->getBetweenDates(start, end);
    double total = 0.0;
    for (const auto& tx : txs) {
        total += tx.getTotalAmount();
    }
    return total;
}

int CheckoutService::getDailyTransactionCount(time_t date) {
    if (!transaction_dal_) return 0;

    struct tm* timeinfo = std::localtime(&date);
    timeinfo->tm_hour = 0;
    timeinfo->tm_min = 0;
    timeinfo->tm_sec = 0;
    time_t start = std::mktime(timeinfo);
    time_t end = start + 86400;

    return static_cast<int>(transaction_dal_->getBetweenDates(start, end).size());
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
