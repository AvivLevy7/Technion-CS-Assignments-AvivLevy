// Ver: 10-4-2025
#ifndef SMASH_COMMAND_H_
#define SMASH_COMMAND_H_

#include <vector>
#include <exception>
#include <string>
#include <unordered_map>
#include "ordered_map.h"

#define COMMAND_MAX_LENGTH (200)
#define COMMAND_MAX_ARGS (20)

class SmallShell;

class Command
{
protected:
    SmallShell *smash;
    int argc;
    char *argv[COMMAND_MAX_ARGS + 2];

public:
    Command();

    virtual ~Command();

    virtual void execute() = 0;

    virtual void prepare(SmallShell* smash, const char* cmd_line);
};

class JobsList
{
public:
    struct JobEntry
    {
        JobEntry() = default;
        JobEntry(pid_t pid, const char* cmd_line): pid(pid), cmd(cmd_line) {}
        pid_t pid;
        std::string cmd;
    };

    typedef ordered_map<int, JobEntry>::iterator iterator;
private:
    ordered_map<int, JobEntry> jobs;
    unsigned long maxId;
public:
    JobsList();

    ~JobsList();

    void addJob(const JobEntry& job);
    void addJob(JobEntry&& job);

    void killAllJobs();

    void removeFinishedJobs();

    JobEntry* getJobById(int jobId);

    void removeJobById(int jobId);

    JobEntry* getLastJob(int *lastJobId);

    int getMaxJobId() const;

    size_t getSize() const;

    iterator begin();

    iterator end();
};

class BuiltInCommand : public Command {
};
    
class ExternalCommand : public Command {
    std::string originalCommand;
    bool isBackground, isComplex;
public:
    ExternalCommand(const char* cmd_line, bool isBackground, bool isComplex);
    void execute() override;
};

class RedirectionCommand : public Command {
    std::string command, path;
    bool isAppend;
public:
    void prepare(SmallShell* smash, const char* cmd_line) override;
    void execute() override;
};

class PipeCommand : public Command {
    std::string firstCommand, secondCommand;
    bool isErr;
public:
    void prepare(SmallShell* smash, const char* cmd_line) override;
    void execute() override;
};

class DiskUsageCommand : public Command {
public:
    void execute() override;
};

class WhoAmICommand : public Command {
public:
    void execute() override;
};

class NetInfoCommand : public Command {
public:
    void execute() override;
};

class ChangePromptCommand : public BuiltInCommand {
public:
    void execute() override;
};

class ChangeDirCommand : public BuiltInCommand {
    static char* lastDir;
public:
    void execute() override;
};

class GetCurrDirCommand : public BuiltInCommand {
public:
    void execute() override;
};

class ShowPidCommand : public BuiltInCommand {
public:
    void execute() override;
};

class QuitCommand : public BuiltInCommand {
public:
    void execute() override;
};

class JobsCommand : public BuiltInCommand {
public:
    void execute() override;
};

class KillCommand : public BuiltInCommand {
public:
    void execute() override;
};

class ForegroundCommand : public BuiltInCommand {
public:
    void execute() override;
};

class AliasCommand : public BuiltInCommand {
public:
    void execute() override;
};

class UnAliasCommand : public BuiltInCommand {
public:
    void execute() override;
};

class UnSetEnvCommand : public BuiltInCommand {
public:
    void execute() override;
};

class WatchProcCommand : public BuiltInCommand {
public:
    void execute() override;
};

typedef Command*(*CommandCreator)(void);

class SmallShell
{
private:
    std::string prompt;
    JobsList jobs;
    std::unordered_map<std::string, CommandCreator> commands;
    ordered_map<std::string, std::string> aliases;
    pid_t foregroundProcess;
    SmallShell();
public:
    Command* CreateCommand(const char *cmd_line);

    SmallShell(SmallShell const &) = delete;     // disable copy ctor
    void operator=(SmallShell const &) = delete; // disable = operator
    static SmallShell& getInstance()             // make SmallShell singleton
    {
        static SmallShell instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

    ~SmallShell();

    void executeCommand(const char *cmd_line);

    const std::string& getPrompt() const noexcept;
    void setPrompt(const std::string &prompt) noexcept;

    JobsList *getJobs() noexcept;

    bool hasAlias(const std::string& alias);
    void addAlias(const std::string& alias, const std::string& value);
    bool removeAlias(const std::string& alias);
    void printAliases();

    bool isReserved(const std::string& word);

    void waitFor(pid_t pid);
    void killForeground();
};

#endif // SMASH_COMMAND_H_
