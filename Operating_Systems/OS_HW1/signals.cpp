#include <iostream>
#include <signal.h>
#include "signals.h"
#include "Commands.h"

using namespace std;

void ctrlCHandler(int sig_num) {
    std::cout << "smash: got ctrl-C" << std::endl;
    
    SmallShell& smash = SmallShell::getInstance();
    smash.killForeground();
}