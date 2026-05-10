#include "Kernel.h"
#include <iostream>
#include <string>
#include <unistd.h>
#include <cstdlib>
using namespace std;

void displayBootSequence() {
    cout << "\n\nStarting Shaheer-Ibrahim-Abdullah Operating System\n\n";
    
    cout << "Initializing hardware components";
    cout.flush();
    for(int i=0;i<3;i++) {
        cout << ".";
        cout.flush();
        sleep(1);
    }
    cout << "\n";
    
    cout << "Loading Kernel modules";
    cout.flush();
    for(int i=0;i<3;i++) {
        cout << ".";
        cout.flush();
        sleep(1);
    }
    cout << "\n";
    
    cout << "Mounting file systems";
    cout.flush();
    for(int i=0;i<3;i++) {
        cout << ".";
        cout.flush();
        sleep(1);
    }
    cout << "\n";
    
    cout << "System boot complete!!\n\n";
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Usage: ./osSimulator <RAM_GB> <STORAGE_GB> <CORES>\n";
        return 1;
    }

    double ramGb = atof(argv[1]);
    double storageGb = atof(argv[2]);
    int cores = atoi(argv[3]);

    if (ramGb <= 0 || storageGb <= 0 || cores <= 0) {
        cerr << "Error: Hardware specifications must be positive values.\n";
        return 1;
    }

    displayBootSequence();

    Kernel systemKernel(ramGb, storageGb, cores);
    systemKernel.bootSystem();

    cout << "Entering Kernel Mode";
    cout.flush();
    for(int i=0;i<3;i++) {
        cout << ".";
        cout.flush();
        sleep(1);
    }
    cout << "\n";
    
    systemKernel.setOperatingMode(OperatingMode::KernelMode);

    systemKernel.runOsLoop();

    return 0;
}
