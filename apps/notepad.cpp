#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <mutex>
#include <chrono>
#include <atomic>
using namespace std;

string documentContent;
mutex contentMutex;
atomic<bool> isRunning(true);

void autoSaveThread() {
    while (isRunning) {
        this_thread::sleep_for(chrono::seconds(5));
        lock_guard<mutex> lock(contentMutex);
        ofstream outFile("autosave.txt");
        if (outFile.is_open()) {
            outFile << documentContent;
            outFile.close();
        }
    }
}

int main() {
    cout << "Notepad Task Started\n";
    cout << "Instructions:\n";
    cout << "  - Type your text below.\n";
    cout << "  - Your work is automatically saved every 5 seconds to 'autosave.txt'.\n";
    cout << "  - Type 'exit' on a NEW line to save and close the editor.\n";

    thread saver(autoSaveThread);

    string line;
    while (true) {
        getline(cin, line);
        if (line == "exit") {
            isRunning = false;
            cout << "Saving and closing notepad...\n";
            break;
        }
        
        lock_guard<mutex> lock(contentMutex);
        documentContent += line + "\n";
    }

    saver.join();
    return 0;
}
