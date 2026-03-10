#include <backend/models/product.hpp>
#include <ctime>

namespace hero::models {

Product::Product(ProductId id, const std::string& name, const std::string& sku,
                 double price, int stock, ProductId categoryId, ProductId supplierId)
    : id_(id), name_(name), sku_(sku), price_(price), stock_(stock),
      category_id_(categoryId), supplier_id_(supplierId) {
    created_at_ = std::time(nullptr);
    updated_at_ = created_at_;
}

Product::Product(ProductId id, const std::string& name, const std::string& sku,
                 double buy_price, double sell_price, double margin, int stock, 
                 ProductId categoryId, ProductId supplierId, const std::string& barcode, 
                 time_t expiration_date)
    : id_(id), name_(name), sku_(sku), buy_price_(buy_price), price_(sell_price), margin_(margin), 
      stock_(stock), category_id_(categoryId), supplier_id_(supplierId), barcode_(barcode),
      expiration_date_(expiration_date) {
    created_at_ = std::time(nullptr);
    updated_at_ = created_at_;
}

bool Product::addStock(int quantity) {
    if (quantity < 0) return false;
    stock_ += quantity;
    updateTimestamp();
    return true;
}

bool Product::removeStock(int quantity) {
    if (quantity < 0 || quantity > stock_) return false;
    stock_ -= quantity;
    updateTimestamp();
    return true;
}

json Product::toJson() const {
    json j;
    j["id"] = id_;
    j["name"] = name_;
    j["sku"] = sku_;
    j["buy_price"] = buy_price_;
    j["price"] = price_;
    j["margin"] = margin_;
    j["barcode"] = barcode_;
    j["expiration_date"] = expiration_date_;
    j["stock"] = stock_;
    j["category_id"] = category_id_;
    j["supplier_id"] = supplier_id_;
    j["description"] = description_;
    j["created_at"] = created_at_;
    j["updated_at"] = updated_at_;
    return j;
}

Product Product::fromJson(const json& j) {
    Product p;
    if (j.contains("id")) p.id_ = j["id"];
    if (j.contains("name")) p.name_ = j["name"];
    if (j.contains("sku")) p.sku_ = j["sku"];
    if (j.contains("buy_price")) p.buy_price_ = j["buy_price"];
    if (j.contains("price")) p.price_ = j["price"];
    if (j.contains("margin")) p.margin_ = j["margin"];
    if (j.contains("barcode")) p.barcode_ = j["barcode"];
    if (j.contains("expiration_date")) p.expiration_date_ = j["expiration_date"];
    if (j.contains("stock")) p.stock_ = j["stock"];
    if (j.contains("category_id")) p.category_id_ = j["category_id"];
    if (j.contains("supplier_id")) p.supplier_id_ = j["supplier_id"];
    if (j.contains("description")) p.description_ = j["description"];
    if (j.contains("created_at")) p.created_at_ = j["created_at"];
    if (j.contains("updated_at")) p.updated_at_ = j["updated_at"];
    return p;
}

} // namespace hero::models
