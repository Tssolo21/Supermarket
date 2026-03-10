#pragma once

#include <vector>
#include <optional>
#include <string>
#include <backend/models/product.hpp>

namespace hero::dal {

/**
 * @interface IDalProduct
 * @brief Abstract interface for Product Data Access Layer
 */
class IDalProduct {
public:
    using Product = hero::models::Product;
    using ProductId = Product::ProductId;

    virtual ~IDalProduct() = default;

    // Create
    virtual ProductId create(const Product& product) = 0;

    // Read
    virtual std::optional<Product> getById(ProductId id) = 0;
    virtual std::vector<Product> getAll() = 0;
    virtual std::vector<Product> getByCategoryId(ProductId category_id) = 0;
    virtual std::vector<Product> getBySupplierId(ProductId supplier_id) = 0;
    virtual std::optional<Product> getBySku(const std::string& sku) = 0;
    virtual std::vector<Product> searchByName(const std::string& name) = 0;

    // Update
    virtual bool update(const Product& product) = 0;

    // Delete
    virtual bool deleteById(ProductId id) = 0;

    // Stock operations
    virtual bool updateStock(ProductId id, int quantity) = 0;
    virtual bool incrementStock(ProductId id, int quantity) = 0;
    virtual bool decrementStock(ProductId id, int quantity) = 0;

    // Batch operations
    virtual bool bulkCreate(const std::vector<hero::models::Product>& products) = 0;
    virtual bool bulkUpdate(const std::vector<hero::models::Product>& products) = 0;
};

} // namespace hero::dal
