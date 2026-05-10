#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
using namespace std;

atomic<bool> isRunning(true);

int main() {
    cout << "Music Simulation Background Task Started\n";
    cout << "Type 'exit' to stop music playback.\n";

    thread audioThread([]() {
        while (isRunning) {
            cout << "\a" << flush;
            this_thread::sleep_for(chrono::seconds(2));
        }
    });

    string command;
    while (true) {
        cin >> command;
        if (command == "exit") {
            isRunning = false;
            break;
        }
    }

    audioThread.join();
    return 0;
}
