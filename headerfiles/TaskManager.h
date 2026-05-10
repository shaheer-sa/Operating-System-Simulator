#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <string>
#include <map>
#include "ResourceManager.h"
#include "Scheduler.h"

using namespace std;

class TaskManager {
private:
    ResourceManager* resourceManager;
    Scheduler* scheduler;

public:
    TaskManager(ResourceManager* resMgr, Scheduler* sched);
    
    pid_t launchTask(int processId, const string& taskName, const string& executablePath, double requiredRam, double requiredStorage, int requiredCores);
    void handleTaskCompletion(int processId, double releasedRam, double releasedStorage, int releasedCores);
};

#endif
