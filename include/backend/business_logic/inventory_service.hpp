#pragma once

#include <backend/dal/idal_product.hpp>
#include <memory>
#include <vector>

namespace hero::business_logic {

/**
 * @class InventoryService
 * @brief Inventory management and analytics
 */
class InventoryService {
public:
    using Product = hero::models::Product;
    using ProductId = Product::ProductId;

    explicit InventoryService(std::shared_ptr<dal::IDalProduct> dal);

    struct InventoryStats {
        int total_products;
        int low_stock_items;
        int out_of_stock_items;
        double total_inventory_value;
        double average_price;
    };

    InventoryStats getInventoryStats();
    std::vector<Product> getOutOfStockProducts();
    std::vector<Product> getLowStockProducts(int threshold = 10);
    double calculateInventoryValue();
    bool hasEnoughStock(ProductId product_id, int quantity);
    int getAvailableStock(ProductId product_id);

private:
    static constexpr int DEFAULT_LOW_STOCK_THRESHOLD = 10;
    std::shared_ptr<dal::IDalProduct> dal_;
};

} // namespace hero::business_logic
