#ifndef _UTILS_H
#define _UTILS_H

#include <string>

#define SYSERR (-1)

#ifdef DEBUG
#include <iostream>
#define FUNC_ENTRY() \
std::cout << __PRETTY_FUNCTION__ << " --> " << std::endl;

#define FUNC_EXIT() \
std::cout << __PRETTY_FUNCTION__ << " <-- " << std::endl;
#else
#define FUNC_ENTRY()
#define FUNC_EXIT()
#endif

class SmashError : public std::exception {
    std::string error;
public:
    SmashError(const std::string& command ,const std::string& errorMessage);
    const char* what() const noexcept override;
};

class SmashSyscallError : public std::exception {
    std::string error;
public:
    SmashSyscallError(const std::string& syscall);
    const char* what() const noexcept override;
};

class FDBackup {
    int originalFd, fd;
public:
    FDBackup(int fd);
    ~FDBackup() noexcept(false);

    FDBackup(const FDBackup&) = delete;
    FDBackup(FDBackup&&) = delete;
    FDBackup& operator=(const FDBackup&) = delete;
    FDBackup& operator=(FDBackup&&) = delete;
};

struct UserInformation {
    std::string username;
    bool password;
    uid_t userId;
    gid_t groupId;
    std::string comment;
    std::string homeDirectory;
    std::string shell;
};

bool _isnum(const char* str);

std::string _ltrim(const std::string& s);

std::string _rtrim(const std::string& s);

std::string _trim(const std::string& s);

int _parseCommandLine(const char* cmd_line, char** args);

bool _isBackgroundComamnd(const char* cmd_line);

void _removeBackgroundSign(char* cmd_line);

bool _hasEnv(const char* name);

void _unsetEnv(const char* name);

bool _isLegalAlias(const std::string& alias);

bool _isComplexCommand(const std::string& cmd);

void _readUserInformation(const std::string& passwdLine, UserInformation* info);

bool _readLine(int fd, std::string& line);

bool _isAllWhitespace(const std::string& cmd);

void _simpleSleep(size_t seconds);

#endif
