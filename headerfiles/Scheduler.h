#pragma once
#include "PCB.h"
#include <queue>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <memory>
using namespace std;

class Scheduler {
private:
    vector<queue<shared_ptr<ProcessControlBlock>>> multilevelReadyQueue;
    mutex schedulerMutex;
    condition_variable schedulerCondition;

public:
    Scheduler(int numLevels);
    
    void addProcess(shared_ptr<ProcessControlBlock> process, int priorityLevel);
    shared_ptr<ProcessControlBlock> getNextProcess();
    void removeProcess(int processId);
    void executeRoundRobin(int timeQuantumMs);
};
