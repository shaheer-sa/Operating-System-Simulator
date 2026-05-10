#include "TaskManager.h"
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
using namespace std;

TaskManager::TaskManager(ResourceManager* resMgr, Scheduler* sched) 
    : resourceManager(resMgr), scheduler(sched) {}

pid_t TaskManager::launchTask(int processId, const string& taskName, const string& executablePath, double requiredRam, double requiredStorage, int requiredCores) {
    if (!resourceManager->allocateResources(requiredRam, requiredStorage, requiredCores)) {
        return -1;
    }

    auto pcb = make_shared<ProcessControlBlock>(processId, taskName, requiredRam, requiredStorage);
    pcb->setState(ProcessState::Ready);
    
    scheduler->addProcess(pcb, 1);
    
    pid_t pid = fork();
    
    if (pid == 0) {
        vector<const char*> args;
        args.push_back("xterm");
        args.push_back("-e");
        args.push_back(executablePath.c_str());
        args.push_back(nullptr);
        
        execvp("xterm", const_cast<char* const*>(args.data()));
        
        cerr << "Failed to launch task: " << taskName << "\n";
        exit(1);
    } else if (pid > 0) {
        pcb->setState(ProcessState::Running);
        return pid;
    } else {
        cerr << "Fork failed for task: " << taskName << "\n";
        resourceManager->releaseResources(requiredRam, requiredStorage, requiredCores);
        return -1;
    }
    return -1;
}

void TaskManager::handleTaskCompletion(int processId, double releasedRam, double releasedStorage, int releasedCores) {
    scheduler->removeProcess(processId);
    resourceManager->releaseResources(releasedRam, releasedStorage, releasedCores);
}
