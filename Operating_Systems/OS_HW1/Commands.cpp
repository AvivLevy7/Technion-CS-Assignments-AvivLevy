#include <unistd.h>
#include <iostream>
#include <string.h>
#include <vector>
#include <sys/wait.h>
#include <map>
#include <iomanip>
#include <algorithm>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sstream>
#include <memory>
#include <queue>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/syscall.h>
#include "Commands.h"
#include "utilies.h"
#include "environment.h"

#define PREAD 0
#define PWRITE 1

char* ChangeDirCommand::lastDir = nullptr;

Command::Command(): argc(0) {
}

Command::~Command() {
  for (int i = 0; i < argc; i++) free(argv[i]);
}

void Command::prepare(SmallShell* smash, const char *cmd_line) {
  this->smash = smash;
  std::string cmd_s = cmd_line;
  _removeBackgroundSign(&cmd_s[0]);
  argc = _parseCommandLine(cmd_s.c_str(), argv);
}

ExternalCommand::ExternalCommand(const char* cmd_line, bool isBackground, bool isComplex): originalCommand(cmd_line), isBackground(isBackground), isComplex(isComplex) {}

void ExternalCommand::execute() {
  pid_t pid = fork();
  if (SYSERR == pid) throw SmashSyscallError("fork");
  
  if (pid == 0) {
    setpgrp();
    const char* sc = "execvp";
    if (isComplex) {
      std::ostringstream oss;
      oss << argv[0];
      for (int i = 1; i < argc; i++) oss << ' ' << argv[i];
      std::string arg = oss.str();
      sc = "execl";
      execl("/bin/bash", "/bin/bash", "-c", arg.c_str(), nullptr);
    } else {
      execvp(argv[0], argv);
    }
    //No need to check for errors, as `exec` only continues on error.
    if (ENOENT == errno) std::cout << argv[0] << ": command not found" << std::endl;
    else {
      SmashSyscallError error(sc);
      perror(error.what());
    }
    exit(1);
  } else {
    if (isBackground) {
      JobsList* jobs = smash->getJobs();
      jobs->removeFinishedJobs();
      jobs->addJob({ pid, originalCommand.c_str() });
    } else {
      smash->waitFor(pid);
    }
  }
}

void RedirectionCommand::prepare(SmallShell* smash, const char* cmd_line) {
  std::string cmd = cmd_line;
  size_t redirectionIndex = cmd.find_last_of('>');
  isAppend = redirectionIndex && cmd[redirectionIndex - 1] == '>';

  command = cmd.substr(0, isAppend ? redirectionIndex - 1 : redirectionIndex);
  path = _trim(cmd.substr(redirectionIndex + 1));

  this->smash = smash;
}

void RedirectionCommand::execute() {
  FDBackup fdBackup(STDOUT_FILENO);

  if (SYSERR == close(STDOUT_FILENO)) throw SmashSyscallError("close");
  int fd = open(path.c_str(), (isAppend ? O_APPEND : O_TRUNC) | O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
  if (SYSERR == fd) throw SmashSyscallError("open");
  if (fd != STDOUT_FILENO) {
    if (SYSERR == dup2(fd, STDOUT_FILENO)) throw SmashSyscallError("dup2");
    if (SYSERR == close(fd)) throw SmashSyscallError("close");
  }
  smash->executeCommand(command.c_str());
  //The `fd` will be close automatically by the FDBackup
}

void PipeCommand::prepare(SmallShell* smash, const char* cmd_line) {
  std::string cmd = cmd_line;
  size_t pipeIndex = cmd.find_last_of('|');
  isErr = cmd[pipeIndex + 1] == '&';

  firstCommand = cmd.substr(0, pipeIndex);
  secondCommand = cmd.substr(isErr ? pipeIndex + 2 : pipeIndex + 1);

  this->smash = smash;
}

void PipeCommand::execute() {
  int pipes[2];
  if (SYSERR == pipe(pipes)) throw SmashSyscallError("pipe");
  pid_t pid = fork();
  if (SYSERR == pid) throw SmashSyscallError("fork");
  if (pid == 0) {
    setpgrp();
    if (SYSERR == close(pipes[PWRITE])) throw SmashSyscallError("close");
    FDBackup fdBackup(STDIN_FILENO);
    if (SYSERR == dup2(pipes[PREAD], STDIN_FILENO)) throw SmashSyscallError("dup2");
    smash->executeCommand(secondCommand.c_str());
    close(pipes[PREAD]);
    exit(0);
  } else {
    {
      int fd = isErr ? STDERR_FILENO : STDOUT_FILENO;
      if (SYSERR == close(pipes[PREAD])) throw SmashSyscallError("close");
      FDBackup fdBackup(fd);
      if (SYSERR == dup2(pipes[PWRITE], fd)) throw SmashSyscallError("dup2");
      smash->executeCommand(firstCommand.c_str());
      close(pipes[PWRITE]);
    }
    smash->waitFor(pid);
  }
}

void ChangePromptCommand::execute() {
  const char* prompt = argc > 1 ? argv[1] : "smash";
  smash->setPrompt(prompt);
}

void ChangeDirCommand::execute() {
  if (argc > 2) {
    throw SmashError("cd", "too many arguments");
  }
	else if (argc > 1) {
    char* cwd = getcwd(nullptr, 0);
    char* newDir = argv[1];
    if (!strcmp("-", argv[1])) {
      if (nullptr == lastDir) {
        throw SmashError("cd", "OLDPWD not set");
      }
      newDir = lastDir;
    }

    if (SYSERR == chdir(newDir)) throw SmashSyscallError("chdir");

    if (nullptr != lastDir) free(lastDir);
    lastDir = cwd;
  }
}

void GetCurrDirCommand::execute() {
	char* cwd = getcwd(nullptr, 0);
  std::cout << cwd << std::endl;
  free(cwd);
}

void ShowPidCommand::execute() {
	std::cout << "smash pid is " << getpid() << std::endl;
}

void QuitCommand::execute() {
	if (argc > 1 && !strcmp(argv[1], "kill")) {
    JobsList* jobs = smash->getJobs();
    std::cout << "smash: sending SIGKILL signal to " << jobs->getSize() << " jobs:" << std::endl;
    for (auto entry : *jobs) {
      std::cout << entry.second.pid << ": " << entry.second.cmd << std::endl;
      if (SYSERR == kill(entry.second.pid, SIGKILL)) throw SmashSyscallError("kill");
    }
  }

  exit(0);
}

void JobsCommand::execute() {
	JobsList* jobs = smash->getJobs();
  jobs->removeFinishedJobs();
  for (auto entry : *jobs) {
    std::cout << '[' << entry.first << "] " << entry.second.cmd << std::endl;
  }
}

void KillCommand::execute() {
  if (argc != 3 || argv[1][0] != '-' || !_isnum(argv[1] + 1) || !_isnum(argv[2])) {
    throw SmashError("kill", "invalid arguments");
  }

	JobsList* jobs = smash->getJobs();
  int sig = atoi(argv[1] + 1), jobId = atoi(argv[2]);
  auto job = jobs->getJobById(jobId);
  if (nullptr == job) {
    throw SmashError("kill", "job-id " + std::string(argv[2]) + " does not exist");
  }
  
  if (SYSERR == kill(job->pid, sig)) throw SmashSyscallError("kill");
  std::cout << "signal number " << sig << " was sent to pid " << job->pid << std::endl;
}

void ForegroundCommand::execute() {
	if (argc > 2 || (argc > 1 && !_isnum(argv[1]))) {
    throw SmashError("fg", "invalid arguments");
  }
  
  JobsList* jobs = smash->getJobs();
  int jobId = argc > 1 ? atoi(argv[1]) : jobs->getMaxJobId();

  if (argc == 1 && jobs->getSize() == 0) {
    throw SmashError("fg", "jobs list is empty");
  }

  auto job = jobs->getJobById(jobId);

  if (nullptr == job) {
    throw SmashError("fg", "job-id " + std::string(argv[1]) + " does not exist");
  }

  std::cout << job->cmd << " " << job->pid << std::endl;

  smash->waitFor(job->pid);
  jobs->removeJobById(jobId);
}

void AliasCommand::execute() {
	if (argc > 1) {
	  char* equal = strchr(argv[1], '=');
	  if (nullptr == equal) throw SmashError("alias", "invalid alias format");
	  std::string alias(argv[1], equal - argv[1]);
	  if (!_isLegalAlias(alias)) throw SmashError("alias", "invalid alias format");
	  if (equal[1] != '\'') throw SmashError("alias", "invalid alias format");
	  std::string value(equal + 2);
	  for (int i = 2; i < argc; i++) {
	    value.push_back(' ');
	    value.append(argv[i]);
	  }
	  if (value[value.length() - 1] != '\'') throw SmashError("alias", "invalid alias format");
	  value.erase(value.length() - 1);
	  if (std::any_of(value.begin(), value.end(), [](char c){ return c == '\''; })) throw SmashError("alias", "invalid alias format");
	  if (smash->isReserved(alias) || smash->hasAlias(alias)) throw SmashError("alias", alias + " already exists or is a reserved command");
	  smash->addAlias(alias, value);
  } else {
    smash->printAliases();
  }
}

void UnAliasCommand::execute() {
	if (argc == 1) throw SmashError("unalias", "not enough arguments");
  for (int i = 1; i < argc; i++) {
    if (!smash->removeAlias(argv[i])) throw SmashError("unalias", std::string(argv[i]) + " alias does not exist");
  }
}

void UnSetEnvCommand::execute() {
	if (argc == 1) {
    throw SmashError("unsetenv", "not enough arguments");
  }
  Environment& environment = Environment::getInstance();

  for (int i = 1; i < argc; i++) {
    std::string env = argv[i];
    if (!environment.unSetEnv(env)) throw SmashError("unsetenv", env + " does not exist");
  }
}

void WatchProcCommand::execute() {
	if (argc != 2 || !_isnum(argv[1])) {
    throw SmashError("watchproc", "invalid arguments");
  }

  pid_t pid = atoi(argv[1]);
  double cpuPercent, ramMb;
  std::string pidStr = std::string(argv[1]);

  // Check process
  int statLock;
  if (SYSERR == kill(pid, 0)) {
    if (errno == ESRCH) throw SmashError("watchproc", "pid " + pidStr + " does not exist ");
    throw SmashSyscallError("kill");
  }

  // get RAM
  int fd = open(("/proc/" + pidStr + "/status").c_str(), O_RDONLY);
  if (SYSERR == fd) throw SmashSyscallError("open");
  std::string line;
  while (_readLine(fd, line)) {
    if (!line.empty()) {
      if (!strncmp(line.c_str(), "VmRSS:", 6)) {
        int ramKb = atoi(line.substr(line.find_first_not_of(" \t", 6)).c_str());
        ramMb = ramKb / 1024.0;
      }
    }
  }
  if (SYSERR == close(fd)) throw SmashSyscallError("close");

  //get CPU
  std::string word;
  auto getTotalCPU = [&](){
    fd = open("/proc/stat", O_RDONLY);
    if (SYSERR == fd) throw SmashSyscallError("open");
    unsigned long long total = 0;
    while (_readLine(fd, line)) {
      if (!line.empty() && !strncmp(line.c_str(), "cpu", 3) && !isdigit(line[3])) {
        std::istringstream iss(line);
        iss >> word; //skip the 'cpu' word
        unsigned long long time;
        while (iss >> time) total += time;
        break;
      }
    }
    if (SYSERR == close(fd)) throw SmashSyscallError("close");
    return total;
  };
  auto getProcessCPU = [&](){
    fd = open(("/proc/" + pidStr + "/stat").c_str(), O_RDONLY);
    if (SYSERR == fd) throw SmashSyscallError("open");
    _readLine(fd, line);
    std::istringstream iss(line);
    for (int i = 0; i < 13; i++) iss >> word; //The CPU times starts at the 14 word
    unsigned long long utime, stime;
    iss >> utime >> stime;
    if (SYSERR == close(fd)) throw SmashSyscallError("close");
    return utime + stime;
  };
  unsigned long long processCpu = getProcessCPU(), totalCpu = getTotalCPU();
  _simpleSleep(1);
  processCpu = getProcessCPU() - processCpu;
  totalCpu = getTotalCPU() - totalCpu;
  cpuPercent = (processCpu / (double)totalCpu) * 100;

  auto precision = std::cout.precision();
  std::cout << std::fixed << std::setprecision(1) << "PID: " << pid << " | CPU Usage: " << cpuPercent << "% | Memory Usage: " << ramMb << " MB" << std::endl;
  std::cout << std::setprecision(precision);
}

void DiskUsageCommand::execute() {
  if (argc > 2) throw SmashError("du", "too many arguments");
  size_t blocks = 0;
  std::queue<std::string> files;
  files.push(argc > 1 ? argv[1] : ".");
  struct stat st;
  char direntBuf[sizeof(dirent64)];

  if (argc > 1 && (SYSERR == stat(argv[1], &st) || !S_ISDIR(st.st_mode))) throw SmashError("du", "directory " + std::string(argv[1]) + " does not exist");

  while (!files.empty()) {
    for (size_t i = 0, max = files.size(); i < max; i++) {
      std::string file = files.front();
      if (SYSERR == lstat(file.c_str(), &st)) throw SmashSyscallError("lstat");
      if (S_ISDIR(st.st_mode)) {
        int fd = open(file.c_str(), O_DIRECTORY);
        if (SYSERR == fd) throw SmashSyscallError("open");
        while (true) {
          int num = syscall(SYS_getdents64, fd, direntBuf, sizeof(direntBuf));
          if (SYSERR == num) throw SmashSyscallError("getdents64");
          if (num == 0) break;
          dirent64* dirent;
          for (int pos = 0; pos < num; pos += dirent->d_reclen) {
            dirent = (dirent64*)(direntBuf + pos);
            if (!strcmp(".", dirent->d_name) || !strcmp("..", dirent->d_name)) continue;
            files.push(file + "/" + dirent->d_name);
          }
        }
        if (SYSERR == close(fd)) throw SmashSyscallError("close");
      }
      blocks += st.st_blocks;
      files.pop();
    }
  }

  //Blocks are 512 bytes each, and du round it to the next KB.
  std::cout << "Total disk usage: " << (blocks + 1 / 2) << " KB" << std::endl;
}

void WhoAmICommand::execute() {
  int fd = open("/etc/passwd", O_RDONLY);
  if (SYSERR == fd) throw SmashSyscallError("open");
  uid_t uid = geteuid();
  std::string line;
  UserInformation userInfo;

  while (_readLine(fd, line)) {
    if (!line.empty()) {
      _readUserInformation(line, &userInfo);
      if (userInfo.userId == uid) {
        std::cout << userInfo.username << " " << userInfo.homeDirectory << std::endl;
        return;
      }
    }
  }

  if (SYSERR == close(fd)) throw SmashSyscallError("close");
}

void NetInfoCommand::execute() {
  if (argc < 2) throw SmashError("netinfo", "interface not specified");

  //IP Address and network mask
  int nd = socket(AF_INET, SOCK_DGRAM, 0);
  unsigned char addr[4], mask[4], gateway[4];
  if (SYSERR == nd) throw SmashSyscallError("socket");
  ifreq net = {};
  strcpy(net.ifr_name, argv[1]);
  net.ifr_flags = IFF_UP|IFF_BROADCAST|IFF_RUNNING|IFF_MULTICAST;
  if (SYSERR == ioctl(nd, SIOCGIFFLAGS, &net)) throw SmashError("netinfo", std::string("interface ") + argv[1] + " does not exist");

  if (SYSERR == ioctl(nd, SIOCGIFADDR, &net)) throw SmashSyscallError("ioctl");
  memcpy(addr, &((sockaddr_in*)&net.ifr_addr)->sin_addr.s_addr, 4);
  if (SYSERR == ioctl(nd, SIOCGIFNETMASK, &net)) throw SmashSyscallError("ioctl");
  memcpy(mask, &((sockaddr_in*)&net.ifr_netmask)->sin_addr.s_addr, 4);
  if (SYSERR == close(nd)) throw SmashSyscallError("close");

  //Default gateway
  int fd = open("/proc/net/route", O_RDONLY);
  if (SYSERR == fd) throw SmashSyscallError("open");
  std::string line;
  while (_readLine(fd, line)) {
    if (!line.empty()) {
      std::istringstream iss(line);
      std::string ifc, dst, gw;
      iss >> ifc >> dst >> gw;
      if (ifc == argv[1] && dst == "00000000") {
        sscanf(gw.c_str(), "%2hhX%2hhX%2hhX%2hhX", gateway + 3, gateway + 2, gateway + 1, gateway);
        break;
      }
    }
  }
  if (SYSERR == close(fd)) throw SmashSyscallError("close");
#define printIP(ip) (int)ip[0] << '.' << (int)ip[1] << '.' << (int)ip[2] << '.' << (int)ip[3]
  std::cout << "IP Address: " << printIP(addr) << std::endl;
  std::cout << "Subnet Mask: " << printIP(mask) << std::endl;
  std::cout << "Default Gateway: " << printIP(gateway) << std::endl;
#undef printIP
  //DNS Servers
  std::cout << "DNS Servers: ";

  fd = open("/etc/resolv.conf", O_RDONLY);
  if (SYSERR == fd) throw SmashSyscallError("open");
  bool notFirst = false;
  while (_readLine(fd, line)) {
    if (!line.empty()) {
      if (line.size() > 10 && !strncmp(line.c_str(), "nameserver ", 11)) {
        if (notFirst) std::cout << ", ";
        notFirst = true;
        std::cout << _trim(line.substr(11));
      }
    }
  }
  std::cout << std::endl;
  if (SYSERR == close(fd)) throw SmashSyscallError("close");
}

template<typename T> Command* _createCmd() { return new T(); }

SmallShell::SmallShell(): prompt("smash"), foregroundProcess(0)
{
  commands["chprompt"] = _createCmd<ChangePromptCommand>;
  commands["cd"] = _createCmd<ChangeDirCommand>;
  commands["pwd"] = _createCmd<GetCurrDirCommand>;
  commands["showpid"] = _createCmd<ShowPidCommand>;
  commands["quit"] = _createCmd<QuitCommand>;
  commands["jobs"] = _createCmd<JobsCommand>;
  commands["kill"] = _createCmd<KillCommand>;
  commands["fg"] = _createCmd<ForegroundCommand>;
  commands["alias"] = _createCmd<AliasCommand>;
  commands["unalias"] = _createCmd<UnAliasCommand>;
  commands["unsetenv"] = _createCmd<UnSetEnvCommand>;
  commands["watchproc"] = _createCmd<WatchProcCommand>;
}

SmallShell::~SmallShell()
{
}

Command* SmallShell::CreateCommand(const char* cmd_line)
{
  std::string cmd_s = _trim(std::string(cmd_line));
  auto wrapper = [&](Command* cmd) { cmd->prepare(this, cmd_s.c_str()); return cmd; };
  std::size_t specialIndex = cmd_s.find_last_of(">|");

  if (specialIndex != std::string::npos) {
    return wrapper(cmd_s[specialIndex] == '>' ? dynamic_cast<Command*>(new RedirectionCommand()) : new PipeCommand());
  }

  std::string firstWord;

  do {
    size_t firstWhitespace = cmd_s.find_first_of(" \n");
    firstWord = firstWhitespace == std::string::npos ? cmd_s : cmd_s.substr(0, firstWhitespace);
    
    auto it = aliases.find(firstWord);
    if (it != aliases.end()) {
      cmd_s = _trim(it->second + cmd_s.substr(firstWord.length()));
    } else break;
  } while(true);
  
  auto builtinCommand = commands.find(firstWord);

  if (builtinCommand != commands.end())
  {
    return wrapper(builtinCommand->second());
  }
  else
  {
    //TODO: check if to run just like builtin commands
    if (firstWord == "du") return wrapper(new DiskUsageCommand());
    if (firstWord == "whoami") return wrapper(new WhoAmICommand());
    if (firstWord == "netinfo") return wrapper(new NetInfoCommand());
    return wrapper(new ExternalCommand(cmd_line, _isBackgroundComamnd(cmd_s.c_str()), _isComplexCommand(cmd_s)));
  }

  return nullptr;
}

void SmallShell::executeCommand(const char* cmd_line)
{
  std::unique_ptr<Command> cmd(CreateCommand(cmd_line));
  
  try {
    cmd->execute();
  } catch(const SmashError& e) {
    std::cerr << e.what() << std::endl;
  } catch(const SmashSyscallError& e) {
    perror(e.what());
  }
}

const std::string& SmallShell::getPrompt() const noexcept {
  return prompt;
}
void SmallShell::setPrompt(const std::string& prompt) noexcept {
  this->prompt = prompt;
}

JobsList* SmallShell::getJobs() noexcept {
  return &jobs;
}

bool SmallShell::hasAlias(const std::string& alias) {
  return aliases.find(alias) != aliases.end();
}

void SmallShell::addAlias(const std::string& alias, const std::string& value) {
  aliases.insert(alias, value);
}
bool SmallShell::removeAlias(const std::string& alias) {
  return aliases.erase(alias);
}

bool SmallShell::isReserved(const std::string& word) {
  return commands.find(word) != commands.end() && word != "du" && word != "whoami" && word != "netinfo";
}

void SmallShell::printAliases() {
  for (auto entry : aliases) {
    std::cout << entry.first << "='" << entry.second << "'" << std::endl;
  }
}

void SmallShell::waitFor(pid_t pid) {
  foregroundProcess = pid;
  if (SYSERR == waitpid(pid, nullptr, 0)) {
    foregroundProcess = 0;
    throw SmashSyscallError("waitpid");
  }
  foregroundProcess = 0;
}

void SmallShell::killForeground() {
  FUNC_ENTRY()
  if (foregroundProcess) {
    kill(foregroundProcess, SIGKILL);
    waitpid(foregroundProcess, NULL, 0);
    std::cout << "smash: process " << foregroundProcess << " was killed" << std::endl;
    foregroundProcess = 0;
  }
  FUNC_EXIT()
}

JobsList::JobsList(): maxId(0) {}

JobsList::~JobsList() {}

void JobsList::addJob(const JobEntry& job) {
  jobs.insert(++maxId, job);
}

void JobsList::addJob(JobEntry&& job) {
  jobs.insert(++maxId, static_cast<JobEntry&&>(job));
}

void JobsList::killAllJobs() {
  for (auto entry : jobs) {
    //TODO: printing
    kill(entry.second.pid, SIGKILL);
  }
  jobs.clear();
  maxId = 0;
}

bool compareJobs(const std::pair<int, JobsList::JobEntry>& first, const std::pair<int, JobsList::JobEntry>& second) {
  return first.first < second.first;
}

void JobsList::removeFinishedJobs() {
  std::list<int> finishedJobs;
  for (auto entry : jobs) {
    int stat;
    switch (waitpid(entry.second.pid, &stat, WNOHANG)) {
      case SYSERR: throw SmashSyscallError("waitpid");
      case 0: break;
      default:
        finishedJobs.push_back(entry.first);
        break;
    }
  }

  bool removedMaxId = false;
  for (auto jobId : finishedJobs) {
    jobs.erase(jobId);
    removedMaxId = removedMaxId || (jobId == maxId);
  }

  if (removedMaxId) maxId = jobs.empty() ? 0 : std::max_element(jobs.begin(), jobs.end(), compareJobs)->first;
}

JobsList::JobEntry* JobsList::getJobById(int jobId) {
  auto it = jobs.find(jobId);
  return it == jobs.end() ? nullptr : &it->second;
}

void JobsList::removeJobById(int jobId) {
  jobs.erase(jobId);
  if ((unsigned long)jobId == maxId) {
    maxId = jobs.empty() ? 0 : std::max_element(jobs.begin(), jobs.end(), compareJobs)->first;
  }
}

JobsList::JobEntry* JobsList::getLastJob(int *lastJobId) {
  *lastJobId = maxId;
  return getJobById(maxId);
}

int JobsList::getMaxJobId() const {
  return maxId;
}

size_t JobsList::getSize() const {
  return jobs.size();
}

JobsList::iterator JobsList::begin() {
  return jobs.begin();
}

JobsList::iterator JobsList::end() {
  return jobs.end();
}