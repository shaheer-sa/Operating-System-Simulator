#ifndef KERNEL_H
#define KERNEL_H

#include "ResourceManager.h"
#include "Scheduler.h"
#include "TaskManager.h"
#include <memory>
#include <map>
#include <string>
using namespace std;

enum class OperatingMode {
    KernelMode,
    UserMode
};

class Kernel {
public:
    Kernel(double ram, double storage, int cores);
    void bootSystem();
    void setOperatingMode(OperatingMode mode);
    OperatingMode getOperatingMode() const;
    
    ResourceManager* getResourceManager() const;
    Scheduler* getScheduler() const;
    TaskManager* getTaskManager() const;
    
    void runOsLoop();
    void terminateProcessManually(int processId);
    void cleanupFinishedProcesses();
    void listActiveProcesses();
    void launchTaskOnBoot(string name);
    
private:
    unique_ptr<ResourceManager> resourceManager;
    unique_ptr<Scheduler> scheduler;
    unique_ptr<TaskManager> taskManager;
    OperatingMode currentMode;
    
    struct ProcessInfo {
        int processId;
        string name;
        double ram;
        double storage;
        int cores;
    };
    map<pid_t, ProcessInfo> activeProcesses;
};

#endif
