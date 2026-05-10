#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <cstdlib>
#include <iomanip>
#include <ctime>
using namespace std;

atomic<bool> isRunning(true);

int main() {
    srand(time(0));
    cout << "========================================\n";
    cout << "   REAL-TIME SYSTEM RESOURCE MONITOR\n";
    cout << "========================================\n";
    cout << "Status: ACTIVE\n";
    cout << "Type 'exit' to close monitor.\n\n";

    thread monitorThread([]() {
        while (isRunning) {
            double cpu = 5.0 + (rand() % 450) / 10.0;
            double ram = 30.0 + (rand() % 500) / 10.0;
            int temp = 40 + (rand() % 25);

            cout << "\r[CPU: " << fixed << setprecision(1) << cpu << "%] "
                 << "[RAM: " << ram << "%] "
                 << "[TEMP: " << temp << "C] " << flush;
            
            this_thread::sleep_for(chrono::seconds(1));
        }
    });

    string command;
    while (true) {
        cin >> command;
        if (command == "exit") {
            isRunning = false;
            cout << "\nShutting down monitor...\n";
            break;
        }
    }

    monitorThread.join();
    return 0;
}
