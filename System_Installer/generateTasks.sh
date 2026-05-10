#!/bin/bash

mkdir -p tasks

generate_task() {
    local name=$1
    local content=$2
    cat <<EOF > "tasks/${name}.cpp"
#include <iostream>
#include <unistd.h>
#include <ctime>
#include <string>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <thread>
#include <atomic>
#include <chrono>
using namespace std;

int main() {
    $content
    
    cout << "\nOperation finished. Type 'exit' to close window.\n";
    string input;
    while(true) {
        cout << "${name} > ";
        cin >> input;
        if(input == "exit") break;
    }
    return 0;
}
EOF
}

# 1. fileCreation (USE STORAGE/)
generate_task "fileCreation" '
    cout << "--- FILE CREATION (Storage Mode) ---\n";
    string filename;
    cout << "New Filename: "; cin >> filename;
    string path = "storage/" + filename;
    cin.ignore(); string c; cout << "Content: "; getline(cin, c);
    ofstream of(path); of << c; of.close();
    cout << "SUCCESS: " << filename << " saved in storage/ folder.\n";
'

# 2. fileDelete (USE STORAGE/)
generate_task "fileDelete" '
    cout << "--- FILE DELETION ---\n";
    system("ls storage/");
    string fn; cout << "Delete which file from storage?: "; cin >> fn;
    string path = "storage/" + fn;
    if(remove(path.c_str()) == 0) cout << "Deleted successfully.\n";
    else cout << "Error: File not found.\n";
'

# 3. fileMove (USE STORAGE/)
generate_task "fileMove" '
    cout << "--- DATA MOVE ---\n";
    system("ls storage/");
    string s, d; cout << "Source file: "; cin >> s; cout << "Dest file: "; cin >> d;
    string spath = "storage/" + s;
    string dpath = "storage/" + d;
    ifstream in(spath);
    if(!in) { cout << "Error: Source file not found.\n"; }
    else {
        ofstream out(dpath, ios::app);
        out << in.rdbuf();
        in.close(); out.close(); remove(spath.c_str());
        cout << "Moved data to " << d << " and purged " << s << ".\n";
    }
'

# 4. fileCopy (USE STORAGE/)
generate_task "fileCopy" '
    system("ls storage/");
    string s, d; cout << "Source: "; cin >> s; cout << "Clone: "; cin >> d;
    ifstream i("storage/"+s);
    if(!i) { cout << "Error: Source file not found.\n"; }
    else {
        ofstream o("storage/"+d);
        o << i.rdbuf();
        cout << "Cloned " << s << " to " << d << " successfully.\n";
    }
'

# 5. fileInformation (USE STORAGE/)
generate_task "fileInformation" '
    cout << "--- STORAGE CONTENTS ---\n";
    system("ls -lh storage/");
'

# 6. fileEditor (USE STORAGE/)
generate_task "fileEditor" '
    system("ls storage/");
    string fn; cout << "Edit file: "; cin >> fn;
    string path = "storage/" + fn;
    ifstream check(path);
    if(!check.good()) { cout << "Error: File not found.\n"; }
    else {
        check.close();
        ofstream o(path, ios::app);
        cin.ignore();
        string l;
        cout << "Append text (type DONE on a new line to finish):\n";
        while(getline(cin, l) && l != "DONE") { o << l << "\n"; }
        o.close();
        cout << "File updated.\n";
    }
'

# 7. textSearch (SEARCH IN STORAGE/)
generate_task "textSearch" '
    cout << "--- TEXT SEARCH IN STORAGE ---\n";
    string keyword;
    cout << "Enter keyword: "; cin >> keyword;
    string cmd = "grep -rn " + keyword + " storage/ 2>/dev/null";
    cout << "\nSearching for [" << keyword << "] in storage/:\n";
    int ret = system(cmd.c_str());
    if(ret != 0) cout << "No matches found for: " << keyword << "\n";
'

# 8. clock (thread-based with clean exit)
generate_task "clock" '
    atomic<bool> clkRunning(true);
    thread clkThread([&clkRunning](){
        while(clkRunning) {
            time_t n = time(0);
            string ts = ctime(&n);
            if(!ts.empty()) ts.pop_back();
            cout << "\r  Clock: " << ts << "  " << flush;
            sleep(1);
        }
        cout << "\n";
    });
    cout << "System Clock Running. Type exit to stop.\n";
    string cmd; while(cin >> cmd && cmd != "exit");
    clkRunning = false;
    clkThread.join();
    return 0;
'

generate_task "calendar" 'system("cal");'

# 9. timer (with prompt and correct countdown)
generate_task "timer" '
    int s; cout << "Enter countdown seconds: "; cin >> s;
    if(s <= 0) { cout << "Error: Enter a positive number.\n"; }
    else {
        cout << "Timer started for " << s << " seconds.\n";
        for(int i = s; i > 0; i--) {
            cout << "\r  Time remaining: " << i << "s   " << flush;
            sleep(1);
        }
        cout << "\r  TIMER DONE!                   \n";
    }
'

generate_task "randomNumberGenerator" '
    srand(time(0));
    cout << "--- Random Numbers (0-99) ---\n";
    for(int i=0;i<5;i++) cout << (rand()%100) << "\n";
'

# 10. Monitors
generate_task "runningProcessesViewer" '
    cout << left << setw(10) << "PID" << setw(20) << "NAME" << "RAM(GB)\n";
    cout << string(40, '"'"'-'"'"') << "\n";
    ifstream r("process_registry.txt"); string id, n, ra, st;
    bool any = false;
    while(r >> id >> n >> ra >> st) { cout << left << setw(10) << id << setw(20) << n << ra << "\n"; any = true; }
    if(!any) cout << "No processes in registry.\n";
'

generate_task "ramUsageViewer" '
    cout << "--- RAM Usage by Process ---\n";
    ifstream r("process_registry.txt"); string id, n, ra, st;
    bool any = false;
    while(r >> id >> n >> ra >> st) { cout << left << setw(20) << n << ra << " GB\n"; any = true; }
    if(!any) cout << "No processes in registry.\n";
'

# 11. logGenerator
generate_task "logGenerator" '
    time_t ts = time(0);
    string fname = "storage/syslog_" + to_string(ts) + ".txt";
    ofstream lf(fname);
    lf << "=== System Log Generated ===\n";
    lf << "Timestamp: " << ctime(&ts);
    lf << "[INFO] OS Status: Running\n";
    lf << "[INFO] Storage: Active\n";
    ifstream reg("process_registry.txt");
    string line;
    while(getline(reg, line)) lf << "[PROCESS] " << line << "\n";
    lf.close();
    cout << "Log generated: " << fname << "\n";
'

# 12. autoBackup
generate_task "autoBackup" '
    cout << "--- AUTO BACKUP UTILITY ---\n";
    time_t ts = time(0);
    string bdir = "storage/backup_" + to_string(ts);
    system(("mkdir -p " + bdir).c_str());
    cout << "Backing up storage/ to " << bdir << "...\n";
    string cmd = "cp storage/*.txt " + bdir + "/ 2>/dev/null";
    if(system(cmd.c_str()) == 0) {
        cout << "Backup complete. Files saved to: " << bdir << "\n";
    } else {
        cout << "Warning: No .txt files found or backup failed.\n";
        cout << "Backup directory created: " << bdir << "\n";
    }
'

echo "All tasks generated successfully."
