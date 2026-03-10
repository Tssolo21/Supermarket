#include <backend/dal/dal_category.hpp>
#include <backend/dal/database.hpp>
#include <sqlite3.h>
#include <sstream>

namespace hero::dal {

using Category = hero::models::Category;
using CategoryId = Category::CategoryId;

CategoryId DalCategory::create(const Category& category) {
    auto& db = Database::getInstance();
    createTableIfNotExists();

    std::stringstream sql;
    sql << "INSERT INTO categories (name, description, created_at) VALUES (?, ?, ?)";

    auto stmt = db.prepareStatement(sql.str());
    if (!stmt) return -1;

    sqlite3_bind_text(stmt, 1, category.getName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, category.getDescription().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 3, category.getCreatedAt());

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        db.finalizeStatement(stmt);
        return -1;
    }

    CategoryId id = sqlite3_last_insert_rowid(db.getRawConnection());
    db.finalizeStatement(stmt);
    return id;
}

std::optional<Category> DalCategory::getById(CategoryId id) {
    auto& db = Database::getInstance();
    createTableIfNotExists();

    auto stmt = db.prepareStatement("SELECT id, name, description, created_at FROM categories WHERE id = ?");
    if (!stmt) return std::nullopt;

    sqlite3_bind_int64(stmt, 1, id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        Category c(
            sqlite3_column_int64(stmt, 0),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2))
        );
        db.finalizeStatement(stmt);
        return c;
    }

    db.finalizeStatement(stmt);
    return std::nullopt;
}

std::vector<Category> DalCategory::getAll() {
    auto& db = Database::getInstance();
    createTableIfNotExists();

    auto stmt = db.prepareStatement("SELECT id, name, description, created_at FROM categories");
    if (!stmt) return {};

    std::vector<Category> results;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Category c(
            sqlite3_column_int64(stmt, 0),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2))
        );
        results.push_back(c);
    }

    db.finalizeStatement(stmt);
    return results;
}

bool DalCategory::update(const Category& category) {
    auto& db = Database::getInstance();
    createTableIfNotExists();

    auto stmt = db.prepareStatement("UPDATE categories SET name = ?, description = ? WHERE id = ?");
    if (!stmt) return false;

    sqlite3_bind_text(stmt, 1, category.getName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, category.getDescription().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 3, category.getId());

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    db.finalizeStatement(stmt);
    return success;
}

bool DalCategory::deleteById(CategoryId id) {
    auto& db = Database::getInstance();
    createTableIfNotExists();

    auto stmt = db.prepareStatement("DELETE FROM categories WHERE id = ?");
    if (!stmt) return false;

    sqlite3_bind_int64(stmt, 1, id);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    db.finalizeStatement(stmt);
    return success;
}

std::vector<Category> DalCategory::searchByName(const std::string& name) {
    auto& db = Database::getInstance();
    createTableIfNotExists();

    auto stmt = db.prepareStatement("SELECT id, name, description, created_at FROM categories WHERE name LIKE ?");
    if (!stmt) return {};

    std::string search_pattern = "%" + name + "%";
    sqlite3_bind_text(stmt, 1, search_pattern.c_str(), -1, SQLITE_TRANSIENT);

    std::vector<Category> results;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Category c(
            sqlite3_column_int64(stmt, 0),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2))
        );
        results.push_back(c);
    }

    db.finalizeStatement(stmt);
    return results;
}

bool DalCategory::createTableIfNotExists() {
    auto& db = Database::getInstance();

    std::string sql = R"(
        CREATE TABLE IF NOT EXISTS categories (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL UNIQUE,
            description TEXT,
            created_at INTEGER
        )
    )";

    return db.execute(sql) == SQLITE_OK;
}

} // namespace hero::dal
