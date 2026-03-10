#pragma once

#include <string>
#include <ctime>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace hero::models {

/**
 * @class Product
 * @brief Represents a product in the supermarket inventory
 * 
 * Models core product attributes with JSON serialization support.
 * Used across the application for inventory management.
 */
class Product {
public:
    using ProductId = int64_t;

    // Constructor
    Product() = default;
    Product(ProductId id, const std::string& name, const std::string& sku,
            double price, int stock, ProductId categoryId, ProductId supplierId);
    Product(ProductId id, const std::string& name, const std::string& sku,
            double buy_price, double sell_price, double margin, int stock, 
            ProductId categoryId, ProductId supplierId, const std::string& barcode = "", 
            time_t expiration_date = 0);

    // Getters
    ProductId getId() const { return id_; }
    const std::string& getName() const { return name_; }
    const std::string& getSku() const { return sku_; }
    double getBuyPrice() const { return buy_price_; }
    double getPrice() const { return price_; } // Sell price
    double getMargin() const { return margin_; }
    const std::string& getBarcode() const { return barcode_; }
    time_t getExpirationDate() const { return expiration_date_; }
    int getStock() const { return stock_; }
    ProductId getCategoryId() const { return category_id_; }
    ProductId getSupplierId() const { return supplier_id_; }
    const std::string& getDescription() const { return description_; }
    time_t getCreatedAt() const { return created_at_; }
    time_t getUpdatedAt() const { return updated_at_; }

    // Setters
    void setName(const std::string& name) { name_ = name; updateTimestamp(); }
    void setBuyPrice(double price) { buy_price_ = price; updateTimestamp(); }
    void setPrice(double price) { price_ = price; updateTimestamp(); }
    void setMargin(double margin) { margin_ = margin; updateTimestamp(); }
    void setBarcode(const std::string& barcode) { barcode_ = barcode; updateTimestamp(); }
    void setExpirationDate(time_t date) { expiration_date_ = date; updateTimestamp(); }
    void setStock(int stock) { stock_ = stock; updateTimestamp(); }
    void setDescription(const std::string& desc) { description_ = desc; updateTimestamp(); }
    void setSupplierId(ProductId id) { supplier_id_ = id; updateTimestamp(); }

    // Stock management
    bool addStock(int quantity);
    bool removeStock(int quantity);
    bool isInStock() const { return stock_ > 0; }
    bool isLowStock(int threshold = 10) const { return stock_ <= threshold; }

    // JSON serialization
    json toJson() const;
    static Product fromJson(const json& j);

private:
    ProductId id_ = -1;
    std::string name_;
    std::string sku_;
    double buy_price_ = 0.0;
    double price_ = 0.0; // Sell price
    double margin_ = 0.0;
    std::string barcode_;
    time_t expiration_date_ = 0;
    int stock_ = 0;
    ProductId category_id_ = -1;
    ProductId supplier_id_ = -1;
    std::string description_;
    time_t created_at_ = 0;
    time_t updated_at_ = 0;

    void updateTimestamp() {
        updated_at_ = std::time(nullptr);
    }
};

} // namespace hero::models
