#pragma once

#include <string>
#include <vector>
#include <ctime>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace hero::models {

/**
 * @class Transaction
 * @brief Represents a checkout transaction
 */
class Transaction {
public:
    using TransactionId = int64_t;
    using ItemId = int64_t;

    struct TransactionItem {
        ItemId product_id;
        std::string product_name;
        int quantity;
        double unit_price;
        double subtotal;

        json toJson() const;
        static TransactionItem fromJson(const json& j);
    };

    enum class PaymentMethod {
        CASH,
        CARD,
        MOBILE_PAYMENT,
        CHECK
    };

    Transaction() = default;
    explicit Transaction(TransactionId id);

    TransactionId getId() const { return id_; }
    const std::vector<TransactionItem>& getItems() const { return items_; }
    double getTotalAmount() const { return total_amount_; }
    PaymentMethod getPaymentMethod() const { return payment_method_; }
    time_t getTimestamp() const { return timestamp_; }
    const std::string& getCashier() const { return cashier_; }

    void addItem(const TransactionItem& item);
    void setPaymentMethod(PaymentMethod method) { payment_method_ = method; }
    void setCashier(const std::string& cashier) { cashier_ = cashier; }
    void finalize();

    json toJson() const;
    static Transaction fromJson(const json& j);

private:
    TransactionId id_ = -1;
    std::vector<TransactionItem> items_;
    double total_amount_ = 0.0;
    double tax_ = 0.0;
    PaymentMethod payment_method_ = PaymentMethod::CASH;
    time_t timestamp_ = 0;
    std::string cashier_;
};

} // namespace hero::models
