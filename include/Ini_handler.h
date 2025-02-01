#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <sstream>

class IniHandler {
public:
    // Parses the .ini file and stores it in a map
    bool Load(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Unable to open file: " << filename << std::endl;
            return false;
        }

        std::string line, current_section;
        while (std::getline(file, line)) {
            // Trim whitespace
            line = Trim(line);

            // Skip empty lines and comments
            if (line.empty() || line[0] == ';' || line[0] == '#') continue;

            // Check for a section
            if (line.front() == '[' && line.back() == ']') {
                current_section = line.substr(1, line.size() - 2); // Remove brackets
                config_[current_section] = {};
            }
            // Check for a key=value pair
            else if (!current_section.empty()) {
                auto delimiter_pos = line.find('=');
                if (delimiter_pos != std::string::npos) {
                    std::string key = Trim(line.substr(0, delimiter_pos));
                    std::string value = Trim(line.substr(delimiter_pos + 1));
                    config_[current_section][key] = value;
                }
            }
        }
        file.close();
        return true;
    }

    // Retrieves the value for a key in a given section
    std::string Get(const std::string& section, const std::string& key, const std::string& default_value = "") const {
        auto sec_it = config_.find(section);
        if (sec_it != config_.end()) {
            auto key_it = sec_it->second.find(key);
            if (key_it != sec_it->second.end()) {
                return key_it->second;
            }
        }
        return default_value;
    }

private:
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> config_;

    // Helper to trim whitespace from the start and end of a string
    static std::string Trim(const std::string& str) {
        const auto start = str.find_first_not_of(" \t");
        const auto end = str.find_last_not_of(" \t");
        return (start == std::string::npos || end == std::string::npos)
            ? ""
            : str.substr(start, end - start + 1);
    }
};

