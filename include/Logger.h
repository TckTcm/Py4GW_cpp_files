#pragma once
#include <string>
#include <fstream>
#include <mutex>

class Logger {
public:
    static Logger& Instance();

    // Logging methods
    bool LogInfo(const std::string& message);
    bool LogWarning(const std::string& message);
    bool LogError(const std::string& message);

    // Initialize and terminate logging
    void SetLogFile(const std::string& file_path);

private:
    Logger() = default;
    ~Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::mutex log_mutex_;  // Mutex for thread-safe logging
    std::string log_file_path_;

    // Utility for formatted log entry
    std::string GetTimestamp() const;
    bool WriteLog(const std::string& level, const std::string& message);
};
