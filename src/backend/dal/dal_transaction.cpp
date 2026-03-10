#include "backend/dal/dal_transaction.hpp"
#include "backend/dal/database.hpp"
#include <nlohmann/json.hpp>
#include <sstream>

namespace hero::dal {
using Transaction = models::Transaction;

bool DalTransaction::createTableIfNotExists() {
    auto& db = Database::getInstance();
    std::string sql = R"(
        CREATE TABLE IF NOT EXISTS transactions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            total_amount REAL NOT NULL,
            payment_method INTEGER NOT NULL,
            timestamp INTEGER NOT NULL,
            cashier TEXT NOT NULL,
            items_json TEXT NOT NULL
        )
    )";
    return db.execute(sql) == SQLITE_OK;
}

Transaction::TransactionId DalTransaction::create(const Transaction& tx) {
    auto& db = Database::getInstance();
    createTableIfNotExists();

    std::stringstream sql;
    sql << "INSERT INTO transactions (total_amount, payment_method, timestamp, cashier, items_json) "
        << "VALUES (?, ?, ?, ?, ?)";
    
    auto stmt = db.prepareStatement(sql.str());
    if (!stmt) return -1;

    nlohmann::json items_json = nlohmann::json::array();
    for (const auto& item : tx.getItems()) {
        items_json.push_back(item.toJson());
    }
    std::string items_str = items_json.dump();

    sqlite3_bind_double(stmt, 1, tx.getTotalAmount());
    sqlite3_bind_int(stmt, 2, static_cast<int>(tx.getPaymentMethod()));
    sqlite3_bind_int64(stmt, 3, tx.getTimestamp());
    sqlite3_bind_text(stmt, 4, tx.getCashier().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, items_str.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        db.finalizeStatement(stmt);
        return -1;
    }

    auto id = sqlite3_last_insert_rowid(db.getRawConnection());
    db.finalizeStatement(stmt);
    return id;
}

std::vector<Transaction> DalTransaction::getBetweenDates(int64_t start, int64_t end) {
    auto& db = Database::getInstance();
    createTableIfNotExists();

    std::stringstream sql;
    sql << "SELECT id, total_amount, payment_method, timestamp, cashier, items_json FROM transactions "
        << "WHERE timestamp >= ? AND timestamp <= ?";
    
    auto stmt = db.prepareStatement(sql.str());
    if (!stmt) return {};

    sqlite3_bind_int64(stmt, 1, start);
    sqlite3_bind_int64(stmt, 2, end);

    std::vector<Transaction> results;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        nlohmann::json full_tx;
        full_tx["id"] = sqlite3_column_int64(stmt, 0);
        full_tx["total_amount"] = sqlite3_column_double(stmt, 1);
        full_tx["payment_method"] = sqlite3_column_int(stmt, 2);
        full_tx["timestamp"] = sqlite3_column_int64(stmt, 3);
        const char* cashier = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        full_tx["cashier"] = cashier ? cashier : "";
        full_tx["items"] = nlohmann::json::parse(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5)));
        try {
            results.push_back(Transaction::fromJson(full_tx));
        } catch(...) {}
    }
    db.finalizeStatement(stmt);
    return results;
}

std::optional<Transaction> DalTransaction::getById(Transaction::TransactionId id) {
    return std::nullopt; // Omitted
}

} // namespace hero::dal
