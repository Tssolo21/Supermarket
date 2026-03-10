#include <backend/middleware/request_logger.hpp>
#include <iostream>
#include <iomanip>
#include <sstream>

namespace hero::middleware {

RequestLogger::RequestLogger() {
}

RequestLogger::~RequestLogger() {
    if (log_stream_.is_open()) {
        log_stream_.close();
    }
}

RequestLogger& RequestLogger::getInstance() {
    static RequestLogger instance;
    return instance;
}

void RequestLogger::log(LogLevel level, const std::string& method, const std::string& path,
                       int status_code, const std::string& message, const std::string& user_id) {
    if (level < current_level_) return;

    LogEntry entry{
        std::time(nullptr),
        level,
        method,
        path,
        status_code,
        message,
        user_id,
        0
    };

    writeToFile(entry);

    if (console_output_) {
        std::cout << "[" << levelToString(level) << "] "
                  << method << " " << path << " (" << status_code << ") - "
                  << message;
        if (!user_id.empty()) {
            std::cout << " [User: " << user_id << "]";
        }
        std::cout << std::endl;
    }
}

void RequestLogger::logRequest(const std::string& method, const std::string& path, const std::string& user_id) {
    log(LogLevel::LevelInfo, method, path, 0, "Request received", user_id);
}

void RequestLogger::logResponse(const std::string& method, const std::string& path, 
                                int status_code, long duration_ms) {
    std::string level_str = (status_code >= 400) ? "WARNING" : "INFO";
    log(status_code >= 400 ? LogLevel::LevelWarning : LogLevel::LevelInfo, 
        method, path, status_code, 
        "Response sent (" + std::to_string(duration_ms) + "ms)");
}

void RequestLogger::setLogFile(const std::string& file_path) {
    if (log_stream_.is_open()) {
        log_stream_.close();
    }
    log_file_ = file_path;
    log_stream_.open(file_path, std::ios::app);
}

void RequestLogger::setLogLevel(LogLevel level) {
    current_level_ = level;
}

void RequestLogger::enableConsoleOutput(bool enable) {
    console_output_ = enable;
}

#include <fstream>

std::vector<RequestLogger::LogEntry> RequestLogger::getRecentLogs(int count) {
    std::vector<LogEntry> logs;
    std::ifstream file(log_file_);
    if (!file.is_open()) return logs;

    std::string line;
    std::vector<std::string> lines;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    int start = std::max(0, static_cast<int>(lines.size()) - count);
    for (size_t i = start; i < lines.size(); ++i) {
        LogEntry entry{};
        entry.message = lines[i];
        logs.push_back(entry);
    }

    return logs;
}

std::vector<RequestLogger::LogEntry> RequestLogger::getLogsByUser(const std::string& user_id) {
    std::vector<LogEntry> logs;
    std::ifstream file(log_file_);
    if (!file.is_open()) return logs;

    std::string line;
    while (std::getline(file, line)) {
        if (line.find("[User: " + user_id + "]") != std::string::npos) {
            LogEntry entry{};
            entry.message = line;
            entry.user_id = user_id;
            logs.push_back(entry);
        }
    }

    return logs;
}

void RequestLogger::writeToFile(const LogEntry& entry) {
    if (!log_stream_.is_open()) return;

    std::time_t now = entry.timestamp;
    std::tm* timeinfo = std::localtime(&now);
    std::ostringstream oss;

    oss << std::put_time(timeinfo, "%Y-%m-%d %H:%M:%S") << " ["
        << levelToString(entry.level) << "] "
        << entry.method << " " << entry.path << " ("
        << entry.status_code << ") - " << entry.message;

    if (!entry.user_id.empty()) {
        oss << " [User: " << entry.user_id << "]";
    }

    oss << " (Duration: " << entry.duration_ms << "ms)" << std::endl;

    log_stream_ << oss.str();
    log_stream_.flush();
}

std::string RequestLogger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::LevelDebug:   return "DEBUG";
        case LogLevel::LevelInfo:    return "INFO";
        case LogLevel::LevelWarning: return "WARN";
        case LogLevel::LevelError:   return "ERROR";
        default:                return "UNKNOWN";
    }
}

} // namespace hero::middleware
