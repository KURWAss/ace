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
