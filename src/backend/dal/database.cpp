#include <backend/dal/database.hpp>
#include <iostream>
#include <sstream>

namespace hero::dal {

Database& Database::getInstance() {
    static Database instance;
    return instance;
}

Database::~Database() {
    shutdown();
}

bool Database::initialize(const std::string& db_path) {
    if (connected_) {
        setError(1, "Database already initialized");
        return false;
    }

    int rc = sqlite3_open(db_path.c_str(), &db_);
    if (rc != SQLITE_OK) {
        setError(rc, std::string(sqlite3_errmsg(db_)));
        sqlite3_close(db_);
        db_ = nullptr;
        return false;
    }

    connected_ = true;

    // Enable ACID transactions
    execute("PRAGMA journal_mode = WAL;");
    execute("PRAGMA synchronous = FULL;");
    execute("PRAGMA foreign_keys = ON;");

    std::cout << "[DB] Database initialized: " << db_path << std::endl;
    return true;
}

void Database::shutdown() {
    if (db_) {
        if (connected_) {
            sqlite3_close(db_);
            connected_ = false;
        }
        db_ = nullptr;
    }
}

bool Database::isConnected() const {
    return connected_ && db_ != nullptr;
}

bool Database::beginTransaction() {
    return execute("BEGIN TRANSACTION;") == SQLITE_OK;
}

bool Database::commit() {
    return execute("COMMIT;") == SQLITE_OK;
}

bool Database::rollback() {
    return execute("ROLLBACK;") == SQLITE_OK;
}

int Database::execute(const std::string& sql) {
    if (!isConnected()) {
        setError(1, "Database not connected");
        return SQLITE_ERROR;
    }

    char* err_msg = nullptr;
    int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &err_msg);

    if (rc != SQLITE_OK) {
        setError(rc, err_msg ? std::string(err_msg) : "Unknown error");
        sqlite3_free(err_msg);
    }

    return rc;
}

sqlite3_stmt* Database::prepareStatement(const std::string& sql) {
    if (!isConnected()) {
        setError(1, "Database not connected");
        return nullptr;
    }

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        setError(rc, sqlite3_errmsg(db_));
        return nullptr;
    }

    return stmt;
}

void Database::finalizeStatement(sqlite3_stmt* stmt) {
    if (stmt) {
        sqlite3_finalize(stmt);
    }
}

std::string Database::getLastError() const {
    return last_error_;
}

int Database::getLastErrorCode() const {
    return last_error_code_;
}

void Database::setError(int code, const std::string& message) {
    last_error_code_ = code;
    last_error_ = message;
    if (code != SQLITE_OK) {
        std::cerr << "[DB ERROR] Code: " << code << " Message: " << message << std::endl;
    }
}

} // namespace hero::dal
