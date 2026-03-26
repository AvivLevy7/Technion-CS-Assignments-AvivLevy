#include "environment.h"
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "utilies.h"

Environment::Environment() {}

Environment& Environment::getInstance() {
    static Environment instance;
    return instance;
}

void Environment::init() {
    pid_t pid = getpid();
    char environPath[24]; //Enough for the entire path + pid
    sprintf(environPath, "/proc/%d/environ", pid);
    int fd = open(environPath, O_RDONLY);
    if (SYSERR == fd) throw SmashSyscallError("open");
    std::string line;
    while (_readLine(fd, line)) {
        if (!line.empty()) {
            size_t sepIndex = line.find('=');
            data[line.substr(0, sepIndex)] = line.substr(sepIndex + 1);
        }
    }
    if (SYSERR == close(fd)) throw SmashSyscallError("close");
}

bool Environment::hasEnv(const std::string& env) const noexcept {
    return data.find(env) != data.end();
}
const std::string& Environment::getEnv(const std::string& env) const noexcept {
    return data.at(env);
}

bool Environment::unSetEnv(const std::string& env) noexcept {
    if (data.erase(env) == 0) return false;

    //Remove from the real environ also
    char** e = __environ;
    size_t i = 0;
    while (e[i] && (strncmp(env.c_str(), e[i], env.length()) || e[i][env.length()] != '=')) ++i;
    if (e[i]) do {
        i++;
        e[i - 1] = e[i];
    } while(e[i]);

    return true;
}