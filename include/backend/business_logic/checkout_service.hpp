#pragma once

#include <vector>
#include <memory>
#include <future>
#include <optional>
#include <backend/dal/idal_product.hpp>
#include <backend/business_logic/product_service.hpp>
#include <backend/business_logic/inventory_service.hpp>

namespace hero::business_logic {

/**
 * @class CheckoutService
 * @brief Handles checkout and transaction processing
 */
class CheckoutService {
public:
    using Transaction = hero::models::Transaction;
    using TransactionId = Transaction::TransactionId;
    using Product = hero::models::Product;
    using ProductId = Product::ProductId;

    CheckoutService(std::shared_ptr<dal::IDalProduct> dal,
                    std::shared_ptr<ProductService> product_service,
                    std::shared_ptr<InventoryService> inventory_service);

    struct CheckoutResult {
        bool success;
        TransactionId transaction_id;
        double total_amount;
        std::string message;
    };

    // Synchronous checkout
    CheckoutResult processCheckout(const std::vector<std::pair<ProductId, int>>& items,
                                   const std::string& cashier,
                                   Transaction::PaymentMethod method);

    // Asynchronous checkout (non-blocking)
    std::future<CheckoutResult> processCheckoutAsync(
        const std::vector<std::pair<ProductId, int>>& items,
        const std::string& cashier,
        Transaction::PaymentMethod method);

    // Transaction retrieval
    std::optional<Transaction> getTransaction(TransactionId id);
    std::vector<Transaction> getTransactionsByDateRange(time_t start, time_t end);
    std::vector<Transaction> getTransactionsByCashier(const std::string& cashier);

    // Analytics
    double getDailySales(time_t date);
    int getDailyTransactionCount(time_t date);

private:
    void validateCheckoutItems(const std::vector<std::pair<ProductId, int>>& items);
    Transaction createTransaction(const std::vector<std::pair<ProductId, int>>& items,
                                  const std::string& cashier);

    std::shared_ptr<dal::IDalProduct> dal_;
    std::shared_ptr<ProductService> product_service_;
    std::shared_ptr<InventoryService> inventory_service_;
};

} // namespace hero::business_logic
