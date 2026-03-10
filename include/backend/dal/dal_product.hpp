#pragma once

#include <vector>
#include <optional>
#include <backend/dal/idal_product.hpp>

namespace hero::dal {

/**
 * @class DalProduct
 * @brief Data Access Layer for Product entity (SQLite implementation)
 */
class DalProduct : public IDalProduct {
public:
    using Product = hero::models::Product;
    using ProductId = Product::ProductId;

    // Create
    ProductId create(const Product& product) override;

    // Read
    std::optional<Product> getById(ProductId id) override;
    std::vector<Product> getAll() override;
    std::vector<Product> getByCategoryId(ProductId category_id) override;
    std::vector<Product> getBySupplierId(ProductId supplier_id) override;
    std::optional<Product> getBySku(const std::string& sku) override;
    std::vector<Product> searchByName(const std::string& name) override;

    // Update
    bool update(const Product& product) override;

    // Delete
    bool deleteById(ProductId id) override;

    // Stock operations
    bool updateStock(ProductId id, int quantity) override;
    bool incrementStock(ProductId id, int quantity) override;
    bool decrementStock(ProductId id, int quantity) override;

    // Batch operations
    bool bulkCreate(const std::vector<Product>& products) override;
    bool bulkUpdate(const std::vector<Product>& products) override;
    
    // Internal Queries
    std::vector<Product> executeQuery(const std::string& sql);

private:
    static constexpr const char* TABLE_NAME = "products";

    bool createTableIfNotExists();
};

} // namespace hero::dal
