#ifndef PCB_H
#define PCB_H

#include <string>

enum class ProcessState {
    New,
    Ready,
    Running,
    Waiting,
    Terminated
};

class ProcessControlBlock {
private:
    int processId;
    std::string name;
    ProcessState state;
    double requiredRam;
    double requiredStorage;

public:
    ProcessControlBlock(int id, std::string taskName, double ram, double storage);
    
    int getProcessId() const;
    std::string getName() const;
    ProcessState getState() const;
    void setState(ProcessState newState);
    double getRequiredRam() const;
    double getRequiredStorage() const;
};

#endif
