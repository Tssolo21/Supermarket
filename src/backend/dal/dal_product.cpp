#include <backend/dal/dal_product.hpp>
#include <backend/dal/database.hpp>
#include <sqlite3.h>
#include <sstream>
#include <iostream>

namespace hero::dal {

using Product = hero::models::Product;
using ProductId = Product::ProductId;

ProductId DalProduct::create(const Product& product) {
    auto& db = Database::getInstance();
    createTableIfNotExists();

    std::stringstream sql;
    sql << "INSERT INTO " << TABLE_NAME 
        << " (name, sku, buy_price, price, margin, barcode, expiration_date, stock, category_id, supplier_id, description, created_at, updated_at) "
        << "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

    auto stmt = db.prepareStatement(sql.str());
    if (!stmt) return -1;

    sqlite3_bind_text(stmt, 1, product.getName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, product.getSku().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 3, product.getBuyPrice());
    sqlite3_bind_double(stmt, 4, product.getPrice());
    sqlite3_bind_double(stmt, 5, product.getMargin());
    sqlite3_bind_text(stmt, 6, product.getBarcode().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 7, product.getExpirationDate());
    sqlite3_bind_int(stmt, 8, product.getStock());
    sqlite3_bind_int64(stmt, 9, product.getCategoryId());
    sqlite3_bind_int64(stmt, 10, product.getSupplierId());
    sqlite3_bind_text(stmt, 11, product.getDescription().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 12, product.getCreatedAt());
    sqlite3_bind_int64(stmt, 13, product.getUpdatedAt());

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        db.finalizeStatement(stmt);
        return -1;
    }

    ProductId id = sqlite3_last_insert_rowid(db.getRawConnection());
    db.finalizeStatement(stmt);
    return id;
}

std::optional<Product> DalProduct::getById(ProductId id) {
    auto& db = Database::getInstance();
    createTableIfNotExists();

    std::stringstream sql;
    sql << "SELECT id, name, sku, buy_price, price, margin, stock, category_id, supplier_id, barcode, expiration_date, description, created_at, updated_at "
        << "FROM " << TABLE_NAME << " WHERE id = ?";

    auto stmt = db.prepareStatement(sql.str());
    if (!stmt) return std::nullopt;

    sqlite3_bind_int64(stmt, 1, id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        Product p(
            sqlite3_column_int64(stmt, 0), // id
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)), // name
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)), // sku
            sqlite3_column_double(stmt, 3), // buy_price
            sqlite3_column_double(stmt, 4), // price (sell_price)
            sqlite3_column_double(stmt, 5), // margin
            sqlite3_column_int(stmt, 6),    // stock
            sqlite3_column_int64(stmt, 7),  // category_id
            sqlite3_column_int64(stmt, 8),  // supplier_id
            sqlite3_column_type(stmt, 9) == SQLITE_NULL ? "" : reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9)), // barcode
            sqlite3_column_int64(stmt, 10)  // expiration_date
        );
        const char* desc = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 11));
        if (desc) p.setDescription(desc);
        
        db.finalizeStatement(stmt);
        return p;
    }

    db.finalizeStatement(stmt);
    return std::nullopt;
}

std::vector<Product> DalProduct::getAll() {
    auto& db = Database::getInstance();
    createTableIfNotExists();
    
    std::stringstream sql;
    sql << "SELECT id, name, sku, buy_price, price, margin, stock, category_id, supplier_id, barcode, expiration_date, description, created_at, updated_at "
        << "FROM " << TABLE_NAME;

    return executeQuery(sql.str());
}

std::vector<Product> DalProduct::getByCategoryId(ProductId category_id) {
    auto& db = Database::getInstance();
    createTableIfNotExists();
    
    std::stringstream sql;
    sql << "SELECT id, name, sku, buy_price, price, margin, stock, category_id, supplier_id, barcode, expiration_date, description, created_at, updated_at "
        << "FROM " << TABLE_NAME << " WHERE category_id = ?";

    auto stmt = db.prepareStatement(sql.str());
    if (!stmt) return {};

    sqlite3_bind_int64(stmt, 1, category_id);

    std::vector<Product> results;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Product p(
            sqlite3_column_int64(stmt, 0), // id
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)), // name
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)), // sku
            sqlite3_column_double(stmt, 3), // buy_price
            sqlite3_column_double(stmt, 4), // price (sell_price)
            sqlite3_column_double(stmt, 5), // margin
            sqlite3_column_int(stmt, 6),    // stock
            sqlite3_column_int64(stmt, 7),  // category_id
            sqlite3_column_int64(stmt, 8),  // supplier_id
            sqlite3_column_type(stmt, 9) == SQLITE_NULL ? "" : reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9)), // barcode
            sqlite3_column_int64(stmt, 10)  // expiration_date
        );
        const char* desc = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 11));
        if (desc) p.setDescription(desc);
        results.push_back(p);
    }

    db.finalizeStatement(stmt);
    return results;
}

std::vector<Product> DalProduct::getBySupplierId(ProductId supplier_id) {
    auto& db = Database::getInstance();
    createTableIfNotExists();
    
    std::stringstream sql;
    sql << "SELECT id, name, sku, buy_price, price, margin, stock, category_id, supplier_id, barcode, expiration_date, description, created_at, updated_at "
        << "FROM " << TABLE_NAME << " WHERE supplier_id = ?";

    auto stmt = db.prepareStatement(sql.str());
    if (!stmt) return {};

    sqlite3_bind_int64(stmt, 1, supplier_id);

    std::vector<Product> results;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Product p(
            sqlite3_column_int64(stmt, 0), // id
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)), // name
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)), // sku
            sqlite3_column_double(stmt, 3), // buy_price
            sqlite3_column_double(stmt, 4), // price (sell_price)
            sqlite3_column_double(stmt, 5), // margin
            sqlite3_column_int(stmt, 6),    // stock
            sqlite3_column_int64(stmt, 7),  // category_id
            sqlite3_column_int64(stmt, 8),  // supplier_id
            sqlite3_column_type(stmt, 9) == SQLITE_NULL ? "" : reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9)), // barcode
            sqlite3_column_int64(stmt, 10)  // expiration_date
        );
        const char* desc = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 11));
        if (desc) p.setDescription(desc);
        results.push_back(p);
    }

    db.finalizeStatement(stmt);
    return results;
}

std::optional<Product> DalProduct::getBySku(const std::string& sku) {
    auto& db = Database::getInstance();
    createTableIfNotExists();

    std::stringstream sql;
    sql << "SELECT id, name, sku, buy_price, price, margin, stock, category_id, supplier_id, barcode, expiration_date, description, created_at, updated_at "
        << "FROM " << TABLE_NAME << " WHERE sku = ?";

    auto stmt = db.prepareStatement(sql.str());
    if (!stmt) return std::nullopt;

    sqlite3_bind_text(stmt, 1, sku.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        Product p(
            sqlite3_column_int64(stmt, 0), // id
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)), // name
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)), // sku
            sqlite3_column_double(stmt, 3), // buy_price
            sqlite3_column_double(stmt, 4), // price (sell_price)
            sqlite3_column_double(stmt, 5), // margin
            sqlite3_column_int(stmt, 6),    // stock
            sqlite3_column_int64(stmt, 7),  // category_id
            sqlite3_column_int64(stmt, 8),  // supplier_id
            sqlite3_column_type(stmt, 9) == SQLITE_NULL ? "" : reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9)), // barcode
            sqlite3_column_int64(stmt, 10)  // expiration_date
        );
        const char* desc = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 11));
        if (desc) p.setDescription(desc);
        db.finalizeStatement(stmt);
        return p;
    }

    db.finalizeStatement(stmt);
    return std::nullopt;
}

std::vector<Product> DalProduct::searchByName(const std::string& name) {
    auto& db = Database::getInstance();
    createTableIfNotExists();
    
    std::stringstream sql;
    sql << "SELECT id, name, sku, buy_price, price, margin, stock, category_id, supplier_id, barcode, expiration_date, description, created_at, updated_at "
        << "FROM " << TABLE_NAME << " WHERE name LIKE ?";

    auto stmt = db.prepareStatement(sql.str());
    if (!stmt) return {};

    std::string search_pattern = "%" + name + "%";
    sqlite3_bind_text(stmt, 1, search_pattern.c_str(), -1, SQLITE_TRANSIENT);

    std::vector<Product> results;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Product p(
            sqlite3_column_int64(stmt, 0), // id
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)), // name
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)), // sku
            sqlite3_column_double(stmt, 3), // buy_price
            sqlite3_column_double(stmt, 4), // price (sell_price)
            sqlite3_column_double(stmt, 5), // margin
            sqlite3_column_int(stmt, 6),    // stock
            sqlite3_column_int64(stmt, 7),  // category_id
            sqlite3_column_int64(stmt, 8),  // supplier_id
            sqlite3_column_type(stmt, 9) == SQLITE_NULL ? "" : reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9)), // barcode
            sqlite3_column_int64(stmt, 10)  // expiration_date
        );
        const char* desc = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 11));
        if (desc) p.setDescription(desc);
        results.push_back(p);
    }

    db.finalizeStatement(stmt);
    return results;
}

bool DalProduct::update(const Product& product) {
    auto& db = Database::getInstance();
    createTableIfNotExists();

    std::stringstream sql;
    sql << "UPDATE " << TABLE_NAME 
        << " SET name = ?, sku = ?, buy_price = ?, price = ?, margin = ?, barcode = ?, "
        << "expiration_date = ?, stock = ?, category_id = ?, "
        << "supplier_id = ?, description = ?, updated_at = ? WHERE id = ?";

    auto stmt = db.prepareStatement(sql.str());
    if (!stmt) return false;

    sqlite3_bind_text(stmt, 1, product.getName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, product.getSku().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 3, product.getBuyPrice());
    sqlite3_bind_double(stmt, 4, product.getPrice());
    sqlite3_bind_double(stmt, 5, product.getMargin());
    sqlite3_bind_text(stmt, 6, product.getBarcode().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 7, product.getExpirationDate());
    sqlite3_bind_int(stmt, 8, product.getStock());
    sqlite3_bind_int64(stmt, 9, product.getCategoryId());
    sqlite3_bind_int64(stmt, 10, product.getSupplierId());
    sqlite3_bind_text(stmt, 11, product.getDescription().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 12, product.getUpdatedAt());
    sqlite3_bind_int64(stmt, 13, product.getId());

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    db.finalizeStatement(stmt);
    return success;
}

bool DalProduct::deleteById(ProductId id) {
    auto& db = Database::getInstance();
    createTableIfNotExists();

    std::stringstream sql;
    sql << "DELETE FROM " << TABLE_NAME << " WHERE id = ?";

    auto stmt = db.prepareStatement(sql.str());
    if (!stmt) return false;

    sqlite3_bind_int64(stmt, 1, id);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    db.finalizeStatement(stmt);
    return success;
}

bool DalProduct::updateStock(ProductId id, int quantity) {
    auto& db = Database::getInstance();
    createTableIfNotExists();

    std::stringstream sql;
    sql << "UPDATE " << TABLE_NAME << " SET stock = ?, updated_at = ? WHERE id = ?";

    auto stmt = db.prepareStatement(sql.str());
    if (!stmt) return false;

    sqlite3_bind_int(stmt, 1, quantity);
    sqlite3_bind_int64(stmt, 2, std::time(nullptr));
    sqlite3_bind_int64(stmt, 3, id);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    db.finalizeStatement(stmt);
    return success;
}

bool DalProduct::incrementStock(ProductId id, int quantity) {
    auto& db = Database::getInstance();
    createTableIfNotExists();

    std::stringstream sql;
    sql << "UPDATE " << TABLE_NAME << " SET stock = stock + ?, updated_at = ? WHERE id = ?";

    auto stmt = db.prepareStatement(sql.str());
    if (!stmt) return false;

    sqlite3_bind_int(stmt, 1, quantity);
    sqlite3_bind_int64(stmt, 2, std::time(nullptr));
    sqlite3_bind_int64(stmt, 3, id);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    db.finalizeStatement(stmt);
    return success;
}

bool DalProduct::decrementStock(ProductId id, int quantity) {
    auto& db = Database::getInstance();
    createTableIfNotExists();

    std::stringstream sql;
    sql << "UPDATE " << TABLE_NAME << " SET stock = stock - ?, updated_at = ? WHERE id = ? AND stock >= ?";

    auto stmt = db.prepareStatement(sql.str());
    if (!stmt) return false;

    sqlite3_bind_int(stmt, 1, quantity);
    sqlite3_bind_int64(stmt, 2, std::time(nullptr));
    sqlite3_bind_int64(stmt, 3, id);
    sqlite3_bind_int(stmt, 4, quantity);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    db.finalizeStatement(stmt);
    return success;
}

bool DalProduct::bulkCreate(const std::vector<Product>& products) {
    auto& db = Database::getInstance();
    createTableIfNotExists();

    if (!db.beginTransaction()) return false;

    for (const auto& product : products) {
        if (create(product) < 0) {
            db.rollback();
            return false;
        }
    }

    return db.commit();
}

bool DalProduct::bulkUpdate(const std::vector<Product>& products) {
    auto& db = Database::getInstance();
    createTableIfNotExists();

    if (!db.beginTransaction()) return false;

    for (const auto& product : products) {
        if (!update(product)) {
            db.rollback();
            return false;
        }
    }

    return db.commit();
}

bool DalProduct::createTableIfNotExists() {
    auto& db = Database::getInstance();

    std::string sql = R"(
        CREATE TABLE IF NOT EXISTS products (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            sku TEXT NOT NULL UNIQUE,
            buy_price REAL NOT NULL DEFAULT 0.0,
            price REAL NOT NULL,
            margin REAL NOT NULL DEFAULT 0.0,
            barcode TEXT,
            expiration_date INTEGER DEFAULT 0,
            stock INTEGER NOT NULL DEFAULT 0,
            category_id INTEGER,
            supplier_id INTEGER,
            description TEXT,
            created_at INTEGER,
            updated_at INTEGER,
            FOREIGN KEY(category_id) REFERENCES categories(id),
            FOREIGN KEY(supplier_id) REFERENCES suppliers(id)
        )
    )";

    bool ok = db.execute(sql) == SQLITE_OK;
    
    // Migration for existing tables (ignore errors if columns exist)
    db.execute("ALTER TABLE products ADD COLUMN buy_price REAL NOT NULL DEFAULT 0.0");
    db.execute("ALTER TABLE products ADD COLUMN margin REAL NOT NULL DEFAULT 0.0");
    db.execute("ALTER TABLE products ADD COLUMN barcode TEXT");
    db.execute("ALTER TABLE products ADD COLUMN expiration_date INTEGER DEFAULT 0");
    
    return ok;
}

std::vector<Product> DalProduct::executeQuery(const std::string& sql) {
    auto& db = Database::getInstance();
    auto stmt = db.prepareStatement(sql);
    if (!stmt) return {};

    std::vector<Product> results;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Product p(
            sqlite3_column_int64(stmt, 0), // id
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)), // name
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)), // sku
            sqlite3_column_double(stmt, 3), // buy_price
            sqlite3_column_double(stmt, 4), // price (sell_price)
            sqlite3_column_double(stmt, 5), // margin
            sqlite3_column_int(stmt, 6),    // stock
            sqlite3_column_int64(stmt, 7),  // category_id
            sqlite3_column_int64(stmt, 8),  // supplier_id
            sqlite3_column_type(stmt, 9) == SQLITE_NULL ? "" : reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9)), // barcode
            sqlite3_column_int64(stmt, 10)  // expiration_date
        );
        const char* desc = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 11));
        if (desc) p.setDescription(desc);
        results.push_back(p);
    }

    db.finalizeStatement(stmt);
    return results;
}

} // namespace hero::dal
