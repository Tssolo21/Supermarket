#include "frontend/models/product_list_model.hpp"

namespace hero::frontend::models {

ProductListModel::ProductListModel(QObject* parent)
    : QAbstractListModel(parent) {}

int ProductListModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return static_cast<int>(products_.size());
}

QVariant ProductListModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= static_cast<int>(products_.size()))
        return QVariant();

    const auto& product = products_[index.row()];

    switch (role) {
        case IdRole: return QString::fromStdString(product.getId());
        case NameRole: return QString::fromStdString(product.getName());
        case BarcodeRole: return QString::fromStdString(product.getBarcode());
        case BuyPriceRole: return product.getBuyPrice();
        case SellPriceRole: return product.getSellPrice();
        case MarginRole: return product.getMargin();
        case QuantityRole: return product.getQuantity();
        case ExpirationDateRole: return QString::fromStdString(product.getExpirationDate());
        default: return QVariant();
    }
}

QHash<int, QByteArray> ProductListModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[IdRole] = "id";
    roles[NameRole] = "name";
    roles[BarcodeRole] = "barcode";
    roles[BuyPriceRole] = "buyPrice";
    roles[SellPriceRole] = "sellPrice";
    roles[MarginRole] = "margin";
    roles[QuantityRole] = "quantity";
    roles[ExpirationDateRole] = "expirationDate";
    return roles;
}

void ProductListModel::setProducts(const std::vector<hero::models::Product>& products) {
    beginResetModel();
    products_ = products;
    endResetModel();
}

void ProductListModel::updateProduct(int index, const hero::models::Product& product) {
    if (index < 0 || index >= static_cast<int>(products_.size())) return;
    products_[index] = product;
    emit dataChanged(createIndex(index, 0), createIndex(index, 0));
}

void ProductListModel::addProduct(const hero::models::Product& product) {
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    products_.push_back(product);
    endInsertRows();
}

void ProductListModel::removeProduct(int index) {
    if (index < 0 || index >= static_cast<int>(products_.size())) return;
    beginRemoveRows(QModelIndex(), index, index);
    products_.erase(products_.begin() + index);
    endRemoveRows();
}

} // namespace hero::frontend::models
