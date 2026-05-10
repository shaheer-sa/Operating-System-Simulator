#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <mutex>

class ResourceManager {
private:
    double totalRam;
    double totalStorage;
    double availableRam;
    double availableStorage;
    int totalCores;
    std::mutex mtx;

public:
    ResourceManager(double ram, double storage, int cores);
    
    bool allocateResources(double ramNeeded, double storageNeeded, int coresNeeded);
    void releaseResources(double ramReleased, double storageReleased, int coresReleased);
    
    double getAvailableRam() const;
    double getAvailableStorage() const;
    int getAvailableCores() const;
    int getTotalCores() const;
};

#endif
