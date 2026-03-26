#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "Commands.h"
#include "signals.h"
#include "utilies.h"
#include "environment.h"

int main(int argc, char *argv[]) {
    if (signal(SIGINT, ctrlCHandler) == SIG_ERR) {
        perror("smash error: failed to set ctrl-C handler");
    }

    Environment::getInstance().init();

    SmallShell& smash = SmallShell::getInstance();
    JobsList* jobs = smash.getJobs();
    while (true) {
        std::cout << smash.getPrompt() << "> ";
        std::string cmd_line;
        std::getline(std::cin, cmd_line);
        if (std::cin.fail()) break;
        if (_isAllWhitespace(cmd_line)) continue;
        jobs->removeFinishedJobs();
        smash.executeCommand(cmd_line.c_str());
    }
    
    return 0;
}