#include "PCB.h"

ProcessControlBlock::ProcessControlBlock(int id, std::string taskName, double ram, double storage)
    : processId(id), name(taskName), state(ProcessState::New), requiredRam(ram), requiredStorage(storage) {}

int ProcessControlBlock::getProcessId() const { return processId; }
std::string ProcessControlBlock::getName() const { return name; }
ProcessState ProcessControlBlock::getState() const { return state; }
void ProcessControlBlock::setState(ProcessState newState) { state = newState; }
double ProcessControlBlock::getRequiredRam() const { return requiredRam; }
double ProcessControlBlock::getRequiredStorage() const { return requiredStorage; }
