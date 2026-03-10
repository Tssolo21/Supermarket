#include <backend/models/transaction.hpp>
#include <ctime>
#include <numeric>

namespace hero::models {

json Transaction::TransactionItem::toJson() const {
    json j;
    j["product_id"] = product_id;
    j["product_name"] = product_name;
    j["quantity"] = quantity;
    j["unit_price"] = unit_price;
    j["subtotal"] = subtotal;
    return j;
}

Transaction::TransactionItem Transaction::TransactionItem::fromJson(const json& j) {
    TransactionItem item;
    if (j.contains("product_id")) item.product_id = j["product_id"];
    if (j.contains("product_name")) item.product_name = j["product_name"];
    if (j.contains("quantity")) item.quantity = j["quantity"];
    if (j.contains("unit_price")) item.unit_price = j["unit_price"];
    if (j.contains("subtotal")) item.subtotal = j["subtotal"];
    return item;
}

Transaction::Transaction(TransactionId id) : id_(id) {
    timestamp_ = std::time(nullptr);
}

void Transaction::addItem(const TransactionItem& item) {
    items_.push_back(item);
}

void Transaction::finalize() {
    total_amount_ = 0.0;
    for (const auto& item : items_) {
        total_amount_ += item.subtotal;
    }
    // Apply simple tax (10%)
    tax_ = total_amount_ * 0.1;
    total_amount_ += tax_;
}

json Transaction::toJson() const {
    json j;
    j["id"] = id_;
    j["items"] = json::array();
    for (const auto& item : items_) {
        j["items"].push_back(item.toJson());
    }
    j["total_amount"] = total_amount_;
    j["tax"] = tax_;
    j["payment_method"] = static_cast<int>(payment_method_);
    j["timestamp"] = timestamp_;
    j["cashier"] = cashier_;
    return j;
}

Transaction Transaction::fromJson(const json& j) {
    Transaction t;
    if (j.contains("id")) t.id_ = j["id"];
    if (j.contains("items")) {
        for (const auto& item_json : j["items"]) {
            t.items_.push_back(TransactionItem::fromJson(item_json));
        }
    }
    if (j.contains("total_amount")) t.total_amount_ = j["total_amount"];
    if (j.contains("tax")) t.tax_ = j["tax"];
    if (j.contains("payment_method")) t.payment_method_ = static_cast<PaymentMethod>(j["payment_method"].get<int>());
    if (j.contains("timestamp")) t.timestamp_ = j["timestamp"];
    if (j.contains("cashier")) t.cashier_ = j["cashier"];
    return t;
}

} // namespace hero::models
