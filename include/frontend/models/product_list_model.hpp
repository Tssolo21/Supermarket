#pragma once
#include <QAbstractListModel>
#include <vector>
#include "backend/models/product.hpp"

namespace hero::frontend::models {

class ProductListModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum ProductRoles {
        IdRole = Qt::UserRole + 1,
        NameRole,
        BarcodeRole,
        BuyPriceRole,
        SellPriceRole,
        MarginRole,
        QuantityRole,
        ExpirationDateRole
    };

    explicit ProductListModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setProducts(const std::vector<hero::models::Product>& products);
    void updateProduct(int index, const hero::models::Product& product);
    void addProduct(const hero::models::Product& product);
    void removeProduct(int index);

private:
    std::vector<hero::models::Product> products_;
};

} // namespace hero::frontend::models
