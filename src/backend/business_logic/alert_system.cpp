#include "backend/business_logic/alert_system.hpp"
#include "backend/dal/dal_product.hpp"
#include <ctime>
#include <iostream>

namespace hero::business_logic {

void AlertSystem::onNotify(const InventoryEvent& event) {
    if (event.type == InventoryEvent::Type::STOCK_UPDATED) {
        if (event.product.isLowStock()) {
#ifdef QT_CORE_LIB
            emit lowStockAlert(QString::fromStdString(event.product.getName()), event.product.getStock());
#else
            // Console output for non-Qt builds
            std::cout << "[Alert] Low stock: " << event.product.getName() 
                      << " (Stock: " << event.product.getStock() << ")" << std::endl;
#endif
        }
    }
}

void AlertSystem::runDailyCheck() {
    dal::DalProduct dal;
    auto products = dal.getAll();
    time_t now = std::time(nullptr);
    
    for (const auto& product : products) {
        // Check low stock
        if (product.isLowStock()) {
#ifdef QT_CORE_LIB
            emit lowStockAlert(QString::fromStdString(product.getName()), product.getStock());
#else
            std::cout << "[Alert] Daily Check - Low stock: " << product.getName() << std::endl;
#endif
        }
        
        // Check expiration
        if (product.getExpirationDate() > 0 && product.getExpirationDate() <= now) {
            char dateStr[64];
            time_t exp = product.getExpirationDate();
            std::strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", std::localtime(&exp));
#ifdef QT_CORE_LIB
            emit expirationAlert(QString::fromStdString(product.getName()), QString::fromStdString(dateStr));
#else
            std::cout << "[Alert] Daily Check - Expired: " << product.getName() 
                      << " (Date: " << dateStr << ")" << std::endl;
#endif
        }
    }
}

} // namespace hero::business_logic
