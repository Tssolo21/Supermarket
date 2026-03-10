#pragma once
#include <QObject>
#include <QVariantList>
#include <QVariantMap>
#include <QString>
#include <vector>

namespace hero::frontend::controllers {

class CheckoutController : public QObject {
    Q_OBJECT
    Q_PROPERTY(double cartTotal READ cartTotal NOTIFY cartTotalChanged)
public:
    explicit CheckoutController(QObject* parent = nullptr);

    // QML Invokables for Hotkeys & Scanner
    Q_INVOKABLE void onBarcodeScanned(const QString& barcode);
    Q_INVOKABLE void addItem(int productId, int quantity = 1);
    Q_INVOKABLE void removeItem(int index);
    Q_INVOKABLE void clearCart();
    
    // F9 Validation
    Q_INVOKABLE bool validateCheckout(const QString& paymentMethod);
    
    // F12 Discounts
    Q_INVOKABLE void applyDiscount(double percentage);

    double cartTotal() const;

signals:
    void cartTotalChanged();
    void cartUpdated(QVariantList items);
    void checkoutCompleted(bool success, const QString& message);
    void checkoutError(const QString& error);

private:
    void recalculateTotal();

    struct CartItem {
        int productId;
        QString name;
        double price;
        int quantity;
        double subtotal;
    };
    std::vector<CartItem> cart_;
    double discountPercentage_ = 0.0;
};

} // namespace hero::frontend::controllers
