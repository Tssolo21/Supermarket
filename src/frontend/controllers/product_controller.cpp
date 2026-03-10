#include "frontend/controllers/product_controller.hpp"
#include "backend/business_logic/product_service.hpp"
#include <QDebug>

namespace hero::frontend::controllers {

ProductController::ProductController(business_logic::ProductService* service, QObject* parent)
    : QObject(parent), productModel_(new models::ProductListModel(this)), service_(service) {
    refresh();
}

void ProductController::searchProducts(const QString& query) {
    try {
        if (!service_) return;
        auto products = service_->searchProducts(query.toStdString());
        productModel_->setProducts(products);
    } catch (const std::exception& e) {
        emit errorOccurred(QString::fromStdString(e.what()));
    }
}

void ProductController::addProduct(const QString& name, const QString& barcode, double buyPrice, double sellPrice, int quantity, const QString& expirationDate) {
    try {
        if (!service_) return;
        
        // Convert expiration date string to time_t (simplified for now)
        // In a real app, we'd use a proper date parser
        time_t exp_time = 0; 
        
        models::Product p(0, name.toStdString(), barcode.toStdString(), 
                          buyPrice, sellPrice, sellPrice - buyPrice, quantity, 
                          1, 1, barcode.toStdString(), exp_time);
        
        service_->addProduct(p);
        refresh();
        emit productOperationSuccess();
    } catch (const std::exception& e) {
        emit errorOccurred(QString::fromStdString(e.what()));
    }
}

void ProductController::updateProduct(const QString& id, const QString& name, const QString& barcode, double buyPrice, double sellPrice, int quantity, const QString& expirationDate) {
    try {
        if (!service_) return;
        int64_t prodId = id.toLongLong();
        
        time_t exp_time = 0;
        
        models::Product p(prodId, name.toStdString(), barcode.toStdString(), 
                          buyPrice, sellPrice, sellPrice - buyPrice, quantity, 
                          1, 1, barcode.toStdString(), exp_time);
        
        service_->updateProduct(p);
        refresh();
        emit productOperationSuccess();
    } catch (const std::exception& e) {
        emit errorOccurred(QString::fromStdString(e.what()));
    }
}

void ProductController::deleteProduct(const QString& id) {
    try {
        if (!service_) return;
        service_->deleteProduct(id.toLongLong());
        refresh();
        emit productOperationSuccess();
    } catch (const std::exception& e) {
        emit errorOccurred(QString::fromStdString(e.what()));
    }
}

void ProductController::refresh() {
    try {
        if (!service_) return;
        auto products = service_->getAllProducts();
        productModel_->setProducts(products);
    } catch (const std::exception& e) {
        emit errorOccurred(QString::fromStdString(e.what()));
    }
}

} // namespace hero::frontend::controllers
