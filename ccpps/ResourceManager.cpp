#include "ResourceManager.h"
#include <iostream>

ResourceManager::ResourceManager(double ram, double storage, int cores) 
    : totalRam(ram), totalStorage(storage), availableRam(ram), availableStorage(storage), totalCores(cores) {}

bool ResourceManager::allocateResources(double ramNeeded, double storageNeeded, int coresNeeded) {
    std::lock_guard<std::mutex> lock(mtx);
    if (availableRam < ramNeeded) {
        std::cerr << "[Resource Manager] Rejected: Insufficient RAM (Available: " << availableRam << "GB, Needed: " << ramNeeded << "GB)\n";
        return false;
    }
    if (availableStorage < storageNeeded) {
        std::cerr << "[Resource Manager] Rejected: Insufficient Storage (Available: " << availableStorage << "GB, Needed: " << storageNeeded << "GB)\n";
        return false;
    }
    if (totalCores < coresNeeded) {
        std::cerr << "[Resource Manager] Rejected: Insufficient Cores (Available: " << totalCores << ", Needed: " << coresNeeded << ")\n";
        return false;
    }
    
    availableRam -= ramNeeded;
    availableStorage -= storageNeeded;
    totalCores -= coresNeeded;
    return true;
}

void ResourceManager::releaseResources(double ramReleased, double storageReleased, int coresReleased) {
    std::lock_guard<std::mutex> lock(mtx);
    availableRam += ramReleased;
    availableStorage += storageReleased;
    totalCores += coresReleased;
}

double ResourceManager::getAvailableRam() const { return availableRam; }
double ResourceManager::getAvailableStorage() const { return availableStorage; }
int ResourceManager::getAvailableCores() const { return totalCores; }
int ResourceManager::getTotalCores() const { return 4; } // Returns physical limit for UI display
