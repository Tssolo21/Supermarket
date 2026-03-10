#include <backend/dal/dal_supplier.hpp>
#include <backend/dal/database.hpp>
#include <sqlite3.h>
#include <sstream>

namespace hero::dal {

using Supplier = hero::models::Supplier;
using SupplierId = Supplier::SupplierId;

SupplierId DalSupplier::create(const Supplier& supplier) {
    auto& db = Database::getInstance();
    createTableIfNotExists();

    std::stringstream sql;
    sql << "INSERT INTO suppliers (name, email, phone, address, created_at) VALUES (?, ?, ?, ?, ?)";

    auto stmt = db.prepareStatement(sql.str());
    if (!stmt) return -1;

    sqlite3_bind_text(stmt, 1, supplier.getName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, supplier.getEmail().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, supplier.getPhone().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, supplier.getAddress().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 5, supplier.getCreatedAt());

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        db.finalizeStatement(stmt);
        return -1;
    }

    SupplierId id = sqlite3_last_insert_rowid(db.getRawConnection());
    db.finalizeStatement(stmt);
    return id;
}

std::optional<Supplier> DalSupplier::getById(SupplierId id) {
    auto& db = Database::getInstance();
    createTableIfNotExists();

    auto stmt = db.prepareStatement("SELECT id, name, email, phone, address, created_at FROM suppliers WHERE id = ?");
    if (!stmt) return std::nullopt;

    sqlite3_bind_int64(stmt, 1, id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        Supplier s(
            sqlite3_column_int64(stmt, 0),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4))
        );
        db.finalizeStatement(stmt);
        return s;
    }

    db.finalizeStatement(stmt);
    return std::nullopt;
}

std::vector<Supplier> DalSupplier::getAll() {
    auto& db = Database::getInstance();
    createTableIfNotExists();

    auto stmt = db.prepareStatement("SELECT id, name, email, phone, address, created_at FROM suppliers");
    if (!stmt) return {};

    std::vector<Supplier> results;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Supplier s(
            sqlite3_column_int64(stmt, 0),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4))
        );
        results.push_back(s);
    }

    db.finalizeStatement(stmt);
    return results;
}

bool DalSupplier::update(const Supplier& supplier) {
    auto& db = Database::getInstance();
    createTableIfNotExists();

    auto stmt = db.prepareStatement(
        "UPDATE suppliers SET name = ?, email = ?, phone = ?, address = ? WHERE id = ?");
    if (!stmt) return false;

    sqlite3_bind_text(stmt, 1, supplier.getName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, supplier.getEmail().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, supplier.getPhone().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, supplier.getAddress().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 5, supplier.getId());

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    db.finalizeStatement(stmt);
    return success;
}

bool DalSupplier::deleteById(SupplierId id) {
    auto& db = Database::getInstance();
    createTableIfNotExists();

    auto stmt = db.prepareStatement("DELETE FROM suppliers WHERE id = ?");
    if (!stmt) return false;

    sqlite3_bind_int64(stmt, 1, id);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    db.finalizeStatement(stmt);
    return success;
}

std::vector<Supplier> DalSupplier::searchByName(const std::string& name) {
    auto& db = Database::getInstance();
    createTableIfNotExists();

    auto stmt = db.prepareStatement("SELECT id, name, email, phone, address, created_at FROM suppliers WHERE name LIKE ?");
    if (!stmt) return {};

    std::string search_pattern = "%" + name + "%";
    sqlite3_bind_text(stmt, 1, search_pattern.c_str(), -1, SQLITE_TRANSIENT);

    std::vector<Supplier> results;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Supplier s(
            sqlite3_column_int64(stmt, 0),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4))
        );
        results.push_back(s);
    }

    db.finalizeStatement(stmt);
    return results;
}

bool DalSupplier::createTableIfNotExists() {
    auto& db = Database::getInstance();

    std::string sql = R"(
        CREATE TABLE IF NOT EXISTS suppliers (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            email TEXT,
            phone TEXT,
            address TEXT,
            created_at INTEGER
        )
    )";

    return db.execute(sql) == SQLITE_OK;
}

} // namespace hero::dal
