#pragma once
#include <QObject>
#include <QString>
#include <QVariantList>
#include "frontend/models/product_list_model.hpp"

namespace hero::business_logic { class ProductService; }

namespace hero::frontend::controllers {

class ProductController : public QObject {
    Q_OBJECT
    Q_PROPERTY(hero::frontend::models::ProductListModel* productModel READ productModel CONSTANT)

public:
    explicit ProductController(business_logic::ProductService* service, QObject* parent = nullptr);

    hero::frontend::models::ProductListModel* productModel() const { return productModel_; }

    Q_INVOKABLE void searchProducts(const QString& query);
    Q_INVOKABLE void addProduct(const QString& name, const QString& barcode, double buyPrice, double sellPrice, int quantity, const QString& expirationDate);
    Q_INVOKABLE void updateProduct(const QString& id, const QString& name, const QString& barcode, double buyPrice, double sellPrice, int quantity, const QString& expirationDate);
    Q_INVOKABLE void deleteProduct(const QString& id);
    Q_INVOKABLE void refresh();

signals:
    void errorOccurred(const QString& message);
    void productOperationSuccess();

private:
    models::ProductListModel* productModel_;
    business_logic::ProductService* service_;
};

} // namespace hero::frontend::controllers
