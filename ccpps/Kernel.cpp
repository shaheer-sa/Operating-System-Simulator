#include "Kernel.h"
#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <iomanip>
#include <vector>
#include <fstream>
using namespace std;

struct ResourceCost {
    double ram;
    double storage;
    int cores;
};

map<string, ResourceCost> taskResources = {
    {"calculator", {0.05, 0.1, 1}},
    {"notepad", {0.2, 1.0, 1}},
    {"music", {0.3, 5.0, 1}},
    {"monitor", {0.5, 2.0, 1}},
    {"fileEditor", {0.2, 0.5, 1}},
    {"textSearch", {0.4, 10.0, 2}}, // Heavy search uses 2 cores
    {"fileCopy", {0.1, 1.0, 1}},
    {"fileMove", {0.05, 0.1, 1}},
    {"fileDelete", {0.05, 0.1, 1}},
    {"clock", {0.02, 0.05, 1}},
    {"calendar", {0.02, 0.05, 1}},
    {"fileCreation", {0.05, 0.1, 1}},
    {"fileInformation", {0.05, 0.1, 1}},
    {"ramUsageViewer", {0.1, 0.2, 1}},
    {"runningProcessesViewer", {0.1, 0.2, 1}},
    {"logGenerator", {0.1, 1.0, 1}},
    {"randomNumberGenerator", {0.01, 0.02, 1}},
    {"timer", {0.01, 0.02, 1}},
    {"autoBackup", {0.5, 20.0, 2}} // Backup uses 2 cores
};

Kernel::Kernel(double ram, double storage, int cores) {
    resourceManager = make_unique<ResourceManager>(ram, storage, cores);
    scheduler = make_unique<Scheduler>(3);
    taskManager = make_unique<TaskManager>(resourceManager.get(), scheduler.get());
    currentMode = OperatingMode::KernelMode;
}

void Kernel::bootSystem() {
    cout << "System initialized successfully.\n";
    cout << "Initializing Storage (storage/ folder)...\n";
    system("mkdir -p storage");
    
    // Create initial 3 files with your exact data inside storage/
    ofstream f1("storage/file1.txt"); f1 << "My name is Shaheer. This is my Operating System project."; f1.close();
    ofstream f2("storage/file2.txt"); f2 << "My name is Abdullah. This project is for OS Lab."; f2.close();
    ofstream f3("storage/file3.txt"); f3 << "My name is Ibrahim."; f3.close();
    
    cout << "Starting system";
    for (int i = 0; i < 3; i++) { cout << "."; cout.flush(); sleep(1); }
    cout << "\n";
    
    // Launch Clock and Calendar on startup
    launchTaskOnBoot("clock");
    launchTaskOnBoot("calendar");
}

void Kernel::launchTaskOnBoot(string name) {
    double ram = taskResources[name].ram;
    double storage = taskResources[name].storage;
    int cores = taskResources[name].cores;
    string path = "./installed_apps/" + name;
    
    // Static ID for boot tasks
    static int bootId = 999;
    pid_t pid = taskManager->launchTask(bootId--, name, path, ram, storage, cores);
    if (pid > 0) {
        activeProcesses[pid] = {bootId + 1, name, ram, storage, cores};
    }
}

void Kernel::setOperatingMode(OperatingMode mode) {
    currentMode = mode;
}

OperatingMode Kernel::getOperatingMode() const {
    return currentMode;
}

ResourceManager* Kernel::getResourceManager() const {
    return resourceManager.get();
}

Scheduler* Kernel::getScheduler() const {
    return scheduler.get();
}

TaskManager* Kernel::getTaskManager() const {
    return taskManager.get();
}

void Kernel::cleanupFinishedProcesses() {
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (activeProcesses.count(pid)) {
            ProcessInfo info = activeProcesses[pid];
            taskManager->handleTaskCompletion(info.processId, info.ram, info.storage, info.cores);
            activeProcesses.erase(pid);
        }
    }
}

void Kernel::listActiveProcesses() {
    if (activeProcesses.empty()) {
        cout << "No active processes currently running.\n";
        return;
    }
    cout << "\n--- Current Running Processes ---\n";
    cout << left << setw(5) << "ID" << setw(20) << "Name" << setw(15) << "RAM" << setw(10) << "Storage" << "Cores" << endl;
    for (auto const& [pid, info] : activeProcesses) {
        cout << left << setw(5) << info.processId << setw(20) << info.name 
             << fixed << setprecision(2) << setw(10) << info.ram << setw(10) << info.storage << info.cores << endl;
    }
}

void Kernel::runOsLoop() {
    string command;
    int nextProcessId = 1;

    while (true) {
        cleanupFinishedProcesses();
        
        // Update stats file for GUI
        ofstream statsFile("stats.txt");
        if (statsFile.is_open()) {
            statsFile << resourceManager->getAvailableRam() << "\n";
            statsFile << resourceManager->getAvailableStorage() << "\n";
            statsFile << resourceManager->getAvailableCores() << "\n";
            statsFile.close();
        }

        // Update process registry for Backup utility
        ofstream registryFile("process_registry.txt");
        if (registryFile.is_open()) {
            for (auto const& [pid, info] : activeProcesses) {
                registryFile << info.processId << " " << info.name << " " << info.ram << " " << info.storage << "\n";
            }
            registryFile.close();
        }
        
        cout << "\n";
        cout << "============================================================\n";
        cout << "            Shaheer-Ibrahim-Abdullah Operating System       \n";
        cout << "============================================================\n";
        cout << " Mode: " << (currentMode == OperatingMode::KernelMode ? "ADMIN" : "GUEST USER") << "\n";
        cout << "------------------------------------------------------------\n";
        cout << fixed << setprecision(2);
        cout << " SYSTEM RESOURCES: \n";
        cout << " RAM: " << left << setw(10) << (to_string(resourceManager->getAvailableRam()) + " GB") 
             << " Storage: " << left << setw(12) << (to_string(resourceManager->getAvailableStorage()) + " GB") 
             << " Cores: " << resourceManager->getAvailableCores() << " Free\n";
        cout << "------------------------------------------------------------\n";
        
        listActiveProcesses();
        cout << "------------------------------------------------------------\n";

        cout << " COMMAND MENU:\n";
        cout << "  - listApps                  : View all installed applications\n";
        cout << "  - launch [app name]         : Start a new process (e.g. launch notepad)\n";
        cout << "  - killProcess [process id]  : Terminate a running process\n";
        cout << "  - switchMode                : Switch between Admin and Guest mode\n";
        cout << "  - exit                      : Shut down the system\n";
        
        if (currentMode == OperatingMode::KernelMode) {
            cout << "  - addTask [app name]        : Create and compile a new task\n";
        }
        cout << "============================================================\n";

        cout << (currentMode == OperatingMode::KernelMode ? "Admin@OS :> " : "Guest@OS :> ");
        cin >> command;

        if (command == "exit") {
            cout << "Initiating system shutdown";
            cout.flush();
            for(int i=0;i<3;i++) { cout << "."; cout.flush(); sleep(1); }
            cout << "\nShutdown complete.\n";
            break;
        } else if (command == "switchMode") {
            if (currentMode == OperatingMode::KernelMode) {
                currentMode = OperatingMode::UserMode;
                cout << "Security Alert: Switched to Restricted Guest Mode.\n";
            } else {
                currentMode = OperatingMode::KernelMode;
                cout << "Access Granted: Switched to Full Administrator Mode.\n";
            }
        } else if (command == "listProcesses") {
            listActiveProcesses();
        } else if (command == "listApps") {
            cout << "\n--- Installed Applications Catalog ---\n";
            cout << left << setw(20) << "Application" << setw(12) << "RAM Cost" << setw(15) << "Storage Cost" << "Cores" << "\n";
            for (auto const& [name, cost] : taskResources) {
                cout << left << setw(20) << name << setw(12) << (to_string(cost.ram) + "GB") 
                     << setw(15) << (to_string(cost.storage) + "GB") << cost.cores << "\n";
            }
        } else if (command == "addTask" && currentMode == OperatingMode::KernelMode) {
            string newName;
            cin >> newName;
            cout << "Deploying new task source: " << newName << "...\n";
            
            system("mkdir -p tasks installed_apps");
            ofstream outFile("tasks/" + newName + ".cpp");
            outFile << "#include <iostream>\n#include <unistd.h>\nusing namespace std;\nint main() {\n    cout << \"Task '\" << newName << \"' is active.\\n\";\n    cout << \"Press enter to close this terminal...\\n\";\n    cin.get();\n    return 0;\n}\n";
            outFile.close();
            
            string compileCmd = "g++ -std=c++17 tasks/" + newName + ".cpp -o installed_apps/" + newName + " -pthread";
            if (system(compileCmd.c_str()) == 0) {
                taskResources[newName] = {0.10, 0.50, 1};
                cout << "Success: '" << newName << "' is now available in the system bin.\n";
            } else {
                cout << "Deployment Error: Compilation failed for " << newName << ".\n";
            }
        } else if (command == "launch") {
            string taskName;
            cin >> taskName;
            
            if (taskResources.find(taskName) == taskResources.end()) {
                cout << "Error: '" << taskName << "' is not recognized as an installed application.\n";
                continue;
            }

            cout << "Kernel: Allocating resources for " << taskName << "...\n";
            
            string executablePath = "./installed_apps/" + taskName;
            double ramCost = taskResources[taskName].ram;
            double storageCost = taskResources[taskName].storage;
            int coresCost = taskResources[taskName].cores;
            
            pid_t pid = taskManager->launchTask(nextProcessId, taskName, executablePath, ramCost, storageCost, coresCost);
            if (pid > 0) {
                activeProcesses[pid] = {nextProcessId, taskName, ramCost, storageCost, coresCost};
                cout << "Successfully launched " << taskName << " (Process ID: " << nextProcessId << ")\n";
                nextProcessId++;
            } else {
                cout << "System Error: Failed to start process due to insufficient resources.\n";
            }
            
        } else if (command == "killProcess") {
            int pidToKill;
            cin >> pidToKill;
            terminateProcessManually(pidToKill);
        } else {
            cout << "Unknown command. Use 'listApps' to see available software.\n";
        }
    }
}

void Kernel::terminateProcessManually(int processId) {
    for (auto it = activeProcesses.begin(); it != activeProcesses.end(); ++it) {
        if (it->second.processId == processId) {
            kill(it->first, SIGKILL);
            taskManager->handleTaskCompletion(processId, it->second.ram, it->second.storage, it->second.cores);
            cout << "Admin Override: Process " << processId << " has been terminated.\n";
            activeProcesses.erase(it);
            return;
        }
    }
    cout << "Error: Process ID " << processId << " not found in active processes.\n";
}
