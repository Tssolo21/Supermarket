#pragma once

#include <string>
#include <ctime>
#include <memory>
#include <fstream>
#include <sstream>
#include <vector>

namespace hero::middleware {

/**
 * @class RequestLogger
 * @brief Request/Response logging middleware
 * 
 * Logs all HTTP requests and responses for debugging and auditing.
 */
class RequestLogger {
public:
    enum class LogLevel {
        LevelDebug,
        LevelInfo,
        LevelWarning,
        LevelError
    };

    struct LogEntry {
        time_t timestamp;
        LogLevel level;
        std::string method;      // HTTP method (GET, POST, etc.)
        std::string path;        // Request path
        int status_code;         // Response status code
        std::string message;     // Log message
        std::string user_id;     // Authenticated user
        long duration_ms;        // Request duration
    };

    static RequestLogger& getInstance();

    // Logging
    void log(LogLevel level, const std::string& method, const std::string& path,
             int status_code, const std::string& message, const std::string& user_id = "");
    
    void logRequest(const std::string& method, const std::string& path, const std::string& user_id);
    void logResponse(const std::string& method, const std::string& path, 
                     int status_code, long duration_ms);

    // Configuration
    void setLogFile(const std::string& file_path);
    void setLogLevel(LogLevel level);
    void enableConsoleOutput(bool enable);

    // Retrieval
    std::vector<LogEntry> getRecentLogs(int count = 100);
    std::vector<LogEntry> getLogsByUser(const std::string& user_id);

private:
    RequestLogger();
    ~RequestLogger();

    void writeToFile(const LogEntry& entry);
    std::string levelToString(LogLevel level) const;

    std::string log_file_;
    LogLevel current_level_ = LogLevel::LevelInfo;
    bool console_output_ = true;
    std::ofstream log_stream_;
};

} // namespace hero::middleware
