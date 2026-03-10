#pragma once

#ifdef QT_CORE_LIB
#include <QObject>
#include <QString>
#include <QVariantMap>
#endif
#include "backend/core/observer.hpp"
#include "backend/models/product.hpp"

namespace hero::business_logic {

// The event payload for inventory changes
struct InventoryEvent {
    enum class Type {
        STOCK_UPDATED,
        PRODUCT_ADDED,
        PRODUCT_REMOVED
    };
    Type type;
    models::Product product;
};

#ifdef QT_CORE_LIB
class AlertSystem : public QObject, public core::Observer<InventoryEvent> {
    Q_OBJECT
#else
class AlertSystem : public core::Observer<InventoryEvent> {
#endif
public:
    static AlertSystem& getInstance() {
        static AlertSystem instance;
        return instance;
    }

    void onNotify(const InventoryEvent& event) override;

    // Checks current database for expired products and low stocks, emitting signals.
#ifdef QT_CORE_LIB
    Q_INVOKABLE void runDailyCheck();

signals:
    void lowStockAlert(const QString& productName, int currentStock);
    void expirationAlert(const QString& productName, const QString& expirationDate);
#else
    void runDailyCheck();
#endif

private:
    AlertSystem() = default;
#ifdef QT_CORE_LIB
    ~AlertSystem() override = default;
#else
    ~AlertSystem() = default;
#endif
    
    // Disable copy
    AlertSystem(const AlertSystem&) = delete;
    AlertSystem& operator=(const AlertSystem&) = delete;
};

// Global Observable for inventory
class InventoryNotifier : public core::Observable<InventoryEvent> {
public:
    static InventoryNotifier& getInstance() {
        static InventoryNotifier instance;
        return instance;
    }

    void notify(const InventoryEvent& event) {
        notifyObservers(event);
    }

private:
    InventoryNotifier() = default;
};

} // namespace hero::business_logic
