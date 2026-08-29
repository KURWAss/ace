#include "config.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <pwd.h>
#include <unistd.h>

namespace {

std::string GetHomeDirectory() {
    const char* home = std::getenv("HOME");
    if (home != nullptr && home[0] != '\0') {
        return std::string(home);
    }

    struct passwd* pw = getpwuid(getuid());
    if (pw != nullptr && pw->pw_dir != nullptr) {
        return std::string(pw->pw_dir);
    }

    return std::string();
}

}  // namespace

std::string GetConfigPath() {
    std::string home = GetHomeDirectory();
    if (home.empty()) {
        return std::string();
    }
    return home + "/.config/ace/config";
}

void EnsureConfigExists() {
    std::string config_path = GetConfigPath();
    if (config_path.empty()) {
        return;
    }

    if (std::filesystem::exists(config_path)) {
        return;
    }

    std::filesystem::path path(config_path);
    std::filesystem::create_directories(path.parent_path());

    std::ofstream out(config_path);
    if (!out.is_open()) {
        return;
    }

    out << "terminal = alacritty\n";
}

std::string GetTerminalCommand() {
    const std::string default_terminal = "alacritty";

    std::string config_path = GetConfigPath();
    if (config_path.empty()) {
        return default_terminal;
    }

    std::ifstream in(config_path);
    if (!in.is_open()) {
        return default_terminal;
    }

    std::string line;
    while (std::getline(in, line)) {
        size_t separator = line.find('=');
        if (separator == std::string::npos) {
            continue;
        }

        std::string key = line.substr(0, separator);
        std::string value = line.substr(separator + 1);

        auto trim = [](std::string& s) {
            size_t start = s.find_first_not_of(" \t\r\n");
            size_t end = s.find_last_not_of(" \t\r\n");
            if (start == std::string::npos) {
                s.clear();
                return;
            }
            s = s.substr(start, end - start + 1);
        };

        trim(key);
        trim(value);

        if (key == "terminal" && !value.empty()) {
            return value;
        }
    }

    return default_terminal;
}
