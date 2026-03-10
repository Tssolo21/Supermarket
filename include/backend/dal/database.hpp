#pragma once

#include <string>
#include <memory>
#include <sqlite3.h>

namespace hero::dal {

/**
 * @class Database
 * @brief Singleton database coordinator for SQLite
 * 
 * Manages database connections, ACID transactions, and connection pooling.
 * Implements the Singleton pattern to ensure single database instance.
 */
class Database {
public:
    static Database& getInstance();

    // Delete copy operations
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    // Connection management
    bool initialize(const std::string& db_path);
    void shutdown();
    bool isConnected() const;

    // Transaction management
    bool beginTransaction();
    bool commit();
    bool rollback();

    // Query execution
    int execute(const std::string& sql);
    sqlite3_stmt* prepareStatement(const std::string& sql);
    void finalizeStatement(sqlite3_stmt* stmt);

    // Error handling
    std::string getLastError() const;
    int getLastErrorCode() const;

    // Direct access for advanced operations
    sqlite3* getRawConnection() { return db_; }

private:
    Database() = default;
    ~Database();

    sqlite3* db_ = nullptr;
    bool connected_ = false;
    int last_error_code_ = 0;
    std::string last_error_;

    void setError(int code, const std::string& message);
};

} // namespace hero::dal
