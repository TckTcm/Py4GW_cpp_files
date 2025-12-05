#include <GWCA/Logger/Logger.h>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <fstream>

Logger& Logger::Instance() {
    static Logger instance;
    return instance;
}

void Logger::SetLogFile(const std::string& file_path) {
    std::lock_guard<std::mutex> lock(log_mutex_);
    log_file_path_ = file_path;
}

bool Logger::LogInfo(const std::string& message) {
    return WriteLog("INFO", message);
}

bool Logger::LogOk(const std::string& message) {
	return WriteLog("OK", message);
}

bool Logger::LogHook(const std::string& message) {
	return WriteLog("HOOK", message);
}

bool Logger::LogWarning(const std::string& message) {
    return WriteLog("WARNING", message);
}

bool Logger::LogError(const std::string& message) {
    return WriteLog("ERROR", message);
}

bool Logger::LogError(const std::string& message, const std::string& module_name) {
	std::ostringstream oss;
	oss << "[" << module_name << "] " << message;
	return WriteLog("ERROR", oss.str());
}

bool Logger::AssertAddress(const std::string& name, uintptr_t address) {
    if (!address) {
        std::ostringstream oss;
        oss << name << " is null.";
        Logger::Instance().LogError(oss.str());
        return false;
    }
    std::ostringstream oss;
	oss << name << ": " << (void*)address;
	//Logger::Instance().LogOk(oss.str());
    return true;
}

bool Logger::AssertAddress(const std::string& name, uintptr_t address, const std::string& module_name) {
	if (!address) {
		std::ostringstream oss;
		oss << "[" << module_name << "] " << name << " is null.";
		Logger::Instance().LogError(oss.str());
		return false;
	}
	std::ostringstream oss;
	oss << "[" << module_name << "] " << name << ": " << (void*)address;
	//Logger::Instance().LogOk(oss.str());
	return true;
}

bool Logger::AssertHook(const std::string& name, int status) {
    if (status != 0) { // MH_OK is 0, any other value means failure
        std::ostringstream oss;
        oss << "Failed to hook " << name << ". MH_STATUS = " << status;
        Logger::Instance().LogError(oss.str());
        return false;
    }
    std::ostringstream oss;
	oss << name << " hooked.";
	//Logger::Instance().LogHook(oss.str());
    return true;
}

bool Logger::AssertHook(const std::string& name, int status, const std::string& module_name) {
	if (status != 0) { // MH_OK is 0, any other value means failure
		std::ostringstream oss;
		oss << "[" << module_name << "] Failed to hook " << name << ". MH_STATUS = " << status;
		Logger::Instance().LogError(oss.str());
		return false;
	}
	std::ostringstream oss;
	oss << "[" << module_name << "] " << name << " hooked.";
	//Logger::Instance().LogHook(oss.str());
	return true;
}




bool Logger::WriteLog(const std::string& level, const std::string& message) {
    std::lock_guard<std::mutex> lock(log_mutex_);
    if (log_file_path_.empty()) {
		MessageBoxA(nullptr, "Need access to write files to log", "ERROR", MB_OK);
		return false;
	}

    try {
        // Open file, write log, and close it atomically
        std::ofstream log_file(log_file_path_, std::ios::out | std::ios::app);
        if (!log_file.is_open()) {
            MessageBoxA(nullptr, "Need access to write files to log", "ERROR", MB_OK);
            return false;
        }

        log_file << GetTimestamp() << " [" << level << "] " << message << std::endl;
        log_file.close();
    }
    catch (...) {
        // Prevent any exceptions from propagating
    }
	return true;
}

std::string Logger::GetTimestamp() const {
    auto now = std::time(nullptr);
    std::ostringstream timestamp;
    timestamp << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S");
    return timestamp.str();
}
