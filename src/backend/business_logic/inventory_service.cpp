#include <backend/business_logic/inventory_service.hpp>
#include <iostream>
#include <numeric>

namespace hero::business_logic {

using Product = hero::models::Product;
using ProductId = Product::ProductId;

InventoryService::InventoryService(std::shared_ptr<dal::IDalProduct> dal) : dal_(std::move(dal)) {}

InventoryService::InventoryStats InventoryService::getInventoryStats() {
    auto products = dal_->getAll();

    InventoryStats stats{};
    stats.total_products = products.size();
    
    double total_price = 0.0;
    int low_stock_count = 0;
    int out_of_stock_count = 0;
    double total_value = 0.0;

    for (const auto& product : products) {
        if (!product.isInStock()) {
            out_of_stock_count++;
        } else if (product.isLowStock()) {
            low_stock_count++;
        }
        total_price += product.getPrice();
        total_value += product.getPrice() * product.getStock();
    }

    stats.low_stock_items = low_stock_count;
    stats.out_of_stock_items = out_of_stock_count;
    stats.total_inventory_value = total_value;
    stats.average_price = products.empty() ? 0.0 : total_price / products.size();

    return stats;
}

std::vector<Product> InventoryService::getOutOfStockProducts() {
    auto products = dal_->getAll();
    
    std::vector<Product> out_of_stock;
    for (const auto& product : products) {
        if (!product.isInStock()) {
            out_of_stock.push_back(product);
        }
    }
    
    return out_of_stock;
}

std::vector<Product> InventoryService::getLowStockProducts(int threshold) {
    auto products = dal_->getAll();
    std::vector<Product> low_stock;
    for (const auto& product : products) {
        if (product.isLowStock(threshold)) {
            low_stock.push_back(product);
        }
    }
    return low_stock;
}

double InventoryService::calculateInventoryValue() {
    auto products = dal_->getAll();
    return std::accumulate(products.begin(), products.end(), 0.0, [](double sum, const Product& p) {
        return sum + (p.getPrice() * p.getStock());
    });
}

bool InventoryService::hasEnoughStock(ProductId product_id, int quantity) {
    auto product = dal_->getById(product_id);
    
    if (!product) return false;
    return product->getStock() >= quantity;
}

int InventoryService::getAvailableStock(ProductId product_id) {
    auto product = dal_->getById(product_id);
    
    if (!product) return 0;
    return product->getStock();
}

} // namespace hero::business_logic
