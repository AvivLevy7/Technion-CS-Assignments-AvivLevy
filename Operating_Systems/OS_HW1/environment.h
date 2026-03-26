#ifndef _ENVIRONMENT_H
#define _ENVIRONMENT_H

#include <string>
#include <unordered_map>

class Environment {
    std::unordered_map<std::string, std::string> data;
    Environment();
public:
    Environment(const Environment&) = delete;
    Environment& operator=(const Environment&) = delete;
    static Environment& getInstance();
    void init();
    bool hasEnv(const std::string& env) const noexcept;
    const std::string& getEnv(const std::string& env) const noexcept;
    bool unSetEnv(const std::string& env) noexcept;
};

#endif