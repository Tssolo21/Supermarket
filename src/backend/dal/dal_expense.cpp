#include "backend/dal/dal_expense.hpp"
#include "backend/dal/database.hpp"
#include <sstream>
#include <nlohmann/json.hpp>

namespace hero::dal {

using Expense = models::Expense;

bool DalExpense::createTableIfNotExists() {
    auto& db = Database::getInstance();
    std::string sql = R"(
        CREATE TABLE IF NOT EXISTS expenses (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            category TEXT NOT NULL,
            description TEXT,
            amount REAL NOT NULL,
            date INTEGER NOT NULL
        )
    )";
    return db.execute(sql) == SQLITE_OK;
}

Expense::ExpenseId DalExpense::create(const Expense& expense) {
    auto& db = Database::getInstance();
    createTableIfNotExists();

    std::stringstream sql;
    sql << "INSERT INTO expenses (category, description, amount, date) VALUES (?, ?, ?, ?)";
    
    auto stmt = db.prepareStatement(sql.str());
    if (!stmt) return -1;

    sqlite3_bind_text(stmt, 1, expense.getCategory().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, expense.getDescription().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 3, expense.getAmount());
    sqlite3_bind_int64(stmt, 4, expense.getDate());

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        db.finalizeStatement(stmt);
        return -1;
    }

    auto id = sqlite3_last_insert_rowid(db.getRawConnection());
    db.finalizeStatement(stmt);
    return id;
}

std::vector<Expense> DalExpense::getBetweenDates(int64_t start, int64_t end) {
    auto& db = Database::getInstance();
    createTableIfNotExists();

    std::stringstream sql;
    sql << "SELECT id, category, description, amount, date FROM expenses "
        << "WHERE date >= ? AND date <= ?";
    
    auto stmt = db.prepareStatement(sql.str());
    if (!stmt) return {};

    sqlite3_bind_int64(stmt, 1, start);
    sqlite3_bind_int64(stmt, 2, end);

    std::vector<Expense> results;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        nlohmann::json j;
        j["id"] = sqlite3_column_int64(stmt, 0);
        j["category"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        const char* desc = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        j["description"] = desc ? desc : "";
        j["amount"] = sqlite3_column_double(stmt, 3);
        j["date"] = sqlite3_column_int64(stmt, 4);
        
        results.push_back(Expense::fromJson(j));
    }
    db.finalizeStatement(stmt);
    return results;
}

} // namespace hero::dal
