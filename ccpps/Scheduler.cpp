#include "Scheduler.h"
#include <thread>
#include <chrono>
#include <iostream>
using namespace std;

Scheduler::Scheduler(int numLevels) {
    multilevelReadyQueue.resize(numLevels);
}

void Scheduler::addProcess(shared_ptr<ProcessControlBlock> process, int priorityLevel) {
    lock_guard<mutex> lock(schedulerMutex);
    if (priorityLevel >= 0 && priorityLevel < multilevelReadyQueue.size()) {
        multilevelReadyQueue[priorityLevel].push(process);
        schedulerCondition.notify_one();
    }
}

shared_ptr<ProcessControlBlock> Scheduler::getNextProcess() {
    unique_lock<mutex> lock(schedulerMutex);
    
    while (true) {
        for (auto& queue : multilevelReadyQueue) {
            if (!queue.empty()) {
                auto process = queue.front();
                queue.pop();
                return process;
            }
        }
        schedulerCondition.wait(lock);
    }
}

void Scheduler::removeProcess(int processId) {
    lock_guard<mutex> lock(schedulerMutex);
    for (auto& queue : multilevelReadyQueue) {
        int initialSize = queue.size();
        for (int i = 0; i < initialSize; ++i) {
            auto process = queue.front();
            queue.pop();
            if (process->getProcessId() != processId) {
                queue.push(process);
            }
        }
    }
}

void Scheduler::executeRoundRobin(int timeQuantumMs) {
    auto process = getNextProcess();
    if (process) {
        process->setState(ProcessState::Running);
        
        this_thread::sleep_for(chrono::milliseconds(timeQuantumMs));
        
        lock_guard<mutex> lock(schedulerMutex);
        if (process->getState() == ProcessState::Running) {
            process->setState(ProcessState::Ready);
            multilevelReadyQueue[0].push(process); 
        }
    }
}
