#include <unistd.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <sys/wait.h>
#include <iomanip>
#include <algorithm>
#include "utilies.h"

using namespace std;

constexpr size_t MAX_SLEEP_ATTEMPTS = 10;

const std::string WHITESPACE = " \n\r\t\f\v";

SmashError::SmashError(const std::string& command, const std::string& errorMessage) :
error("smash error: " + command + ": " + errorMessage) {}

const char* SmashError::what() const noexcept {
    return error.c_str();
}

SmashSyscallError::SmashSyscallError(const std::string& syscall) :
error("smash error: " + syscall + " failed") {}

const char* SmashSyscallError::what() const noexcept {
    return error.c_str();
}

FDBackup::FDBackup(int fd): originalFd(fd), fd(dup(fd)) {
    if (SYSERR == fd) throw SmashSyscallError("dup");
}

FDBackup::~FDBackup() noexcept(false) {
    if (SYSERR == dup2(fd, originalFd)) throw SmashSyscallError("dup2");
    if (SYSERR == close(fd)) throw SmashSyscallError("close");
}

bool _isnum(const char* str) {
    for(; *str; ++str) {
        if (!isdigit(*str)) return false;
    }
    return true;
}

string _ltrim(const std::string& s) {
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

string _rtrim(const std::string& s) {
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

string _trim(const std::string& s) {
    return _rtrim(_ltrim(s));
}

int _parseCommandLine(const char *cmd_line, char **args) {
    FUNC_ENTRY()
    int i = 0;
    std::istringstream iss(_trim(std::string(cmd_line)).c_str());
    for (std::string s; iss >> s;) {
        args[i] = (char *) malloc(s.length() + 1);
        memset(args[i], 0, s.length() + 1);
        strcpy(args[i], s.c_str());
        args[++i] = NULL;
    }
    FUNC_EXIT()
    return i;
}

bool _isBackgroundComamnd(const char *cmd_line) {
    const string str(cmd_line);
    return str[str.find_last_not_of(WHITESPACE)] == '&';
}

void _removeBackgroundSign(char *cmd_line) {
    const string str(cmd_line);
    // find last character other than spaces
    unsigned int idx = str.find_last_not_of(WHITESPACE);
    // if all characters are spaces then return
    if (idx == string::npos) {
        return;
    }
    // if the command line does not end with & then return
    if (cmd_line[idx] != '&') {
        return;
    }
    // replace the & (background sign) with space and then remove all tailing spaces.
    cmd_line[idx] = ' ';
    // truncate the command line string up to the last non-space character
    cmd_line[str.find_last_not_of(WHITESPACE, idx) + 1] = 0;
}

bool _isLegalAlias(const string& alias) {
    return std::all_of(alias.begin(), alias.end(), [](char c) { return isalnum(c) || c == '_'; });
}

bool _isComplexCommand(const std::string& cmd) {
    return std::any_of(cmd.begin(), cmd.end(), [](char c) { return c == '*' || c == '?'; });
}

void _readUserInformation(const std::string& passwdLine, UserInformation* info) {
#define __update last = next + 1; next = passwdLine.find(':', last)

    size_t last = 0, next = passwdLine.find(':');
    info->username = passwdLine.substr(last, next - last);

    __update;
    info->password = passwdLine[last] == 'x';

    __update;
    info->userId = atoi(passwdLine.c_str() + last);

    __update;
    info->groupId = atoi(passwdLine.c_str() + last);

    __update;
    info->comment = passwdLine.substr(last, next - last);

    __update;
    info->homeDirectory = passwdLine.substr(last, next - last);

    __update;
    info->shell = passwdLine.substr(last);

#undef __update
}

bool _readLine(int fd, std::string& line) {
    line.clear();
    int len;
    char c;
    while (true) {
        len = read(fd, &c, sizeof(char));
        if (SYSERR == len) {
            if (errno == EINTR) continue;
            close(fd);
            throw SmashSyscallError("read");
        }
        if (len == 0 || c == '\n' || c == '\0') break;
        line.insert(line.end(), c);
    }
    return !line.empty() || len > 0;
}

bool _isAllWhitespace(const std::string& cmd) {
    return cmd.find_first_not_of(WHITESPACE) == std::string::npos;
}

void _simpleSleep(size_t seconds) {
    timespec req = {0}, rem = {0};
    req.tv_sec = seconds;
    for (size_t i = 0; i < MAX_SLEEP_ATTEMPTS; i++) {
        if (SYSERR == nanosleep(&req, &rem)) {
            if (errno != EINTR) throw SmashSyscallError("nanosleep");
            req = rem;
        } else break;
    }
}