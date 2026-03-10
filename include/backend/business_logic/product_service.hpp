#pragma once

#include <vector>
#include <optional>
#include <memory>
#include <backend/models/product.hpp>
#include <backend/dal/idal_product.hpp>

namespace hero::business_logic {

/**
 * @class ProductService
 * @brief Business logic for product management
 */
class ProductService {
public:
    using Product = hero::models::Product;
    using ProductId = Product::ProductId;

    explicit ProductService(std::shared_ptr<dal::IDalProduct> dal);

    ProductService(const ProductService&) = delete;
    ProductService& operator=(const ProductService&) = delete;

public:

    // Factory method for creating products
    static Product createProduct(const std::string& name, const std::string& sku,
                                 double price, int stock, ProductId category_id,
                                 ProductId supplier_id);

    // CRUD operations
    ProductId addProduct(const Product& product);
    std::optional<Product> getProduct(ProductId id);
    std::vector<Product> getAllProducts();
    std::vector<Product> getProductsByCategory(ProductId category_id);
    std::vector<Product> getProductsBySupplier(ProductId supplier_id);
    bool updateProduct(const Product& product);
    bool deleteProduct(ProductId id);
    std::vector<Product> searchProducts(const std::string& query);

    // Stock management with business rules
    bool replenishStock(ProductId id, int quantity, ProductId supplier_id);
    bool sellProduct(ProductId id, int quantity);
    std::vector<Product> getLowStockProducts(int threshold = 10);
    int getTotalInventoryValue();

private:
    void validateProduct(const Product& product);
    std::shared_ptr<dal::IDalProduct> dal_;
};

} // namespace hero::business_logic
