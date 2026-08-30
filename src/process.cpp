#include "process.h"

#include <sstream>
#include <unistd.h>
#include <vector>

void LaunchCommand(const std::string& command) {
    std::vector<std::string> tokens;
    std::istringstream stream(command);
    std::string token;
    while (stream >> token) {
        tokens.push_back(token);
    }

    if (tokens.empty()) {
        return;
    }

    pid_t pid = fork();

    if (pid < 0) {
        return;
    }

    if (pid == 0) {
        setsid();

        std::vector<char*> argv;
        for (std::string& t : tokens) {
            argv.push_back(t.data());
        }
        argv.push_back(nullptr);

        execvp(argv[0], argv.data());
        _exit(1);
    }
}
