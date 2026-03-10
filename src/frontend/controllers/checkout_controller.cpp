#include "frontend/controllers/checkout_controller.hpp"
#include "backend/business_logic/checkout_service.hpp"
#include "backend/dal/dal_product.hpp"

namespace hero::frontend::controllers {

CheckoutController::CheckoutController(QObject* parent) : QObject(parent) {}

void CheckoutController::onBarcodeScanned(const QString& barcode) {
    hero::dal::DalProduct dal;
    auto productOpt = dal.getBySkU(barcode.toStdString()); // Using sku as fallback for barcode
    if (!productOpt) {
        // Find using barcode field instead
        auto results = dal.executeQuery("SELECT * FROM products WHERE barcode = '" + barcode.toStdString() + "'");
        if (!results.empty()) {
            productOpt = results[0];
        }
    }
    
    if (productOpt) {
        addItem(productOpt->getId(), 1);
    } else {
        emit checkoutError("Produit non trouvé : " + barcode);
    }
}

void CheckoutController::addItem(int productId, int quantity) {
    hero::dal::DalProduct dal;
    auto productOpt = dal.getById(productId);
    if (!productOpt) {
        emit checkoutError("Produit inconnu.");
        return;
    }

    // Check if item already in cart
    for (auto& item : cart_) {
        if (item.productId == productId) {
            item.quantity += quantity;
            item.subtotal = item.quantity * item.price;
            recalculateTotal();
            return;
        }
    }

    // Add new
    CartItem newItem{
        static_cast<int>(productOpt->getId()),
        QString::fromStdString(productOpt->getName()),
        productOpt->getPrice(),
        quantity,
        productOpt->getPrice() * quantity
    };
    cart_.push_back(newItem);
    recalculateTotal();
}

void CheckoutController::removeItem(int index) {
    if (index >= 0 && index < cart_.size()) {
        cart_.erase(cart_.begin() + index);
        recalculateTotal();
    }
}

void CheckoutController::clearCart() {
    cart_.clear();
    discountPercentage_ = 0.0;
    recalculateTotal();
}

bool CheckoutController::validateCheckout(const QString& paymentMethodStr) {
    if (cart_.empty()) {
        emit checkoutError("Cart is empty.");
        return false;
    }

    hero::business_logic::CheckoutService service;
    std::vector<std::pair<int64_t, int>> checkoutItems;
    for (const auto& item : cart_) {
        checkoutItems.push_back({item.productId, item.quantity});
    }

    auto method = hero::models::Transaction::PaymentMethod::CASH;
    if (paymentMethodStr.toLower() == "card") method = hero::models::Transaction::PaymentMethod::CARD;

    auto result = service.processCheckout(checkoutItems, "FrontDesk", method);
    
    if (result.success) {
        // Include discount visually (backend could be updated to apply discount directly later)
        double realTotal = cartTotal();
        emit checkoutCompleted(true, "Checkout success. Total: $" + QString::number(realTotal, 'f', 2));
        clearCart();
        return true;
    } else {
        emit checkoutError(QString::fromStdString(result.message));
        return false;
    }
}

void CheckoutController::applyDiscount(double percentage) {
    discountPercentage_ = percentage;
    recalculateTotal();
}

double CheckoutController::cartTotal() const {
    double total = 0;
    for (const auto& item : cart_) {
        total += item.subtotal;
    }
    return total * (1.0 - (discountPercentage_ / 100.0));
}

void CheckoutController::recalculateTotal() {
    emit cartTotalChanged();
    
    QVariantList list;
    for (const auto& item : cart_) {
        QVariantMap map;
        map["productId"] = item.productId;
        map["name"] = item.name;
        map["price"] = item.price;
        map["quantity"] = item.quantity;
        map["subtotal"] = item.subtotal;
        list.push_back(map);
    }
    emit cartUpdated(list);
}

} // namespace hero::frontend::controllers
