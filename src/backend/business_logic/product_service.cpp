#include <backend/business_logic/product_service.hpp>
#include <backend/dal/dal_product.hpp>
#include <backend/business_logic/alert_system.hpp>
#include <iostream>

namespace hero::business_logic {

using Product = hero::models::Product;
using ProductId = Product::ProductId;

ProductService::ProductService(std::unique_ptr<dal::IDalProduct> dal) : dal_(std::move(dal)) {}

Product ProductService::createProduct(const std::string& name, const std::string& sku,
                                      double price, int stock, ProductId category_id,
                                      ProductId supplier_id) {
    Product p(0, name, sku, price, stock, category_id, supplier_id);
    return p;
}

ProductId ProductService::addProduct(const Product& product) {
    validateProduct(product);
    
    return dal_->create(product);
}

std::optional<Product> ProductService::getProduct(ProductId id) {
    return dal_->getById(id);
}

std::vector<Product> ProductService::getAllProducts() {
    return dal_->getAll();
}

std::vector<Product> ProductService::getProductsByCategory(ProductId category_id) {
    return dal_->getByCategoryId(category_id);
}

std::vector<Product> ProductService::getProductsBySupplier(ProductId supplier_id) {
    return dal_->getBySupplierId(supplier_id);
}

bool ProductService::updateProduct(const Product& product) {
    validateProduct(product);
    
    return dal_->update(product);
}

bool ProductService::deleteProduct(ProductId id) {
    return dal_->deleteById(id);
}

std::vector<Product> ProductService::searchProducts(const std::string& query) {
    return dal_->searchByName(query);
}

bool ProductService::replenishStock(ProductId id, int quantity, ProductId supplier_id) {
    if (quantity <= 0) {
        std::cerr << "Invalid replenish quantity" << std::endl;
        return false;
    }

    bool success = dal_->incrementStock(id, quantity);
    
    if (success) {
        auto updatedProduct = dal_->getById(id);
        if (updatedProduct) {
            InventoryNotifier::getInstance().notify({InventoryEvent::Type::STOCK_UPDATED, *updatedProduct});
        }
    }
    
    return success;
}

bool ProductService::sellProduct(ProductId id, int quantity) {
    if (quantity <= 0) {
        return false;
    }

    bool success = dal_->decrementStock(id, quantity);
    
    if (success) {
        auto updatedProduct = dal_->getById(id);
        if (updatedProduct) {
            InventoryNotifier::getInstance().notify({InventoryEvent::Type::STOCK_UPDATED, *updatedProduct});
        }
    }
    
    return success;
}

std::vector<Product> ProductService::getLowStockProducts(int threshold) {
    auto products = getAllProducts();
    std::vector<Product> low_stock;
    
    for (const auto& product : products) {
        if (product.isLowStock(threshold)) {
            low_stock.push_back(product);
        }
    }
    
    return low_stock;
}

int ProductService::getTotalInventoryValue() {
    auto products = getAllProducts();
    int total = 0;
    
    for (const auto& product : products) {
        total += static_cast<int>(product.getPrice() * product.getStock());
    }
    
    return total;
}

void ProductService::validateProduct(const Product& product) {
    if (product.getName().empty()) {
        throw std::invalid_argument("Product name cannot be empty");
    }
    if (product.getSku().empty()) {
        throw std::invalid_argument("Product SKU cannot be empty");
    }
    if (product.getPrice() < 0) {
        throw std::invalid_argument("Product price cannot be negative");
    }
    if (product.getStock() < 0) {
        throw std::invalid_argument("Product stock cannot be negative");
    }
}

} // namespace hero::business_logic
