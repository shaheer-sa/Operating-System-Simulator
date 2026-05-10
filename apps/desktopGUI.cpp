#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

// ── Resource costs mirror Kernel.cpp ──────────────────────────────────────────
struct ResourceCost { double ram; double storage; int cores; };

map<string, ResourceCost> taskResources = {
    {"calculator",             {0.05,  0.1,  1}},
    {"notepad",                {0.20,  1.0,  1}},
    {"music",                  {0.30,  5.0,  1}},
    {"monitor",                {0.50,  2.0,  1}},
    {"fileEditor",             {0.20,  0.5,  1}},
    {"textSearch",             {0.40, 10.0,  2}},
    {"fileCopy",               {0.10,  1.0,  1}},
    {"fileMove",               {0.05,  0.1,  1}},
    {"fileDelete",             {0.05,  0.1,  1}},
    {"clock",                  {0.02,  0.05, 1}},
    {"calendar",               {0.02,  0.05, 1}},
    {"fileCreation",           {0.05,  0.1,  1}},
    {"fileInformation",        {0.05,  0.1,  1}},
    {"ramUsageViewer",         {0.10,  0.2,  1}},
    {"runningProcessesViewer", {0.10,  0.2,  1}},
    {"randomNumberGenerator",  {0.01,  0.02, 1}},
    {"timer",                  {0.01,  0.02, 1}},
    {"autoBackup",             {0.50, 20.0,  2}},
};

// ── Per-process tracking ───────────────────────────────────────────────────────
struct ProcessInfo { int id; string name; double ram; double storage; int cores; };

static int    nextProcId    = 1;
static double totalRam      = 4.0;
static double totalStorage  = 256.0;    
static int    totalCores    = 4;

map<pid_t, ProcessInfo> guiProcesses;

// ── Available RAM helper ───────────────────────────────────────────────────────
double getUsedRam() {
    double used = 0;
    for (auto const& [pid, info] : guiProcesses) used += info.ram;
    return used;
}
double getAvailableRam() { return max(0.0, totalRam - getUsedRam()); }

// ── File writers ───────────────────────────────────────────────────────────────
void updateRegistry() {
    ofstream reg("process_registry.txt");
    if (!reg.is_open()) return;
    for (auto const& [pid, info] : guiProcesses)
        reg << info.id << " " << info.name << " "
            << fixed << setprecision(2) << info.ram << " " << info.storage << "\n";
}

void updateStats() {
    double usedRam = 0, usedStorage = 0; int usedCores = 0;
    for (auto const& [pid, info] : guiProcesses) {
        usedRam     += info.ram;
        usedStorage += info.storage;
        usedCores   += info.cores;
    }
    ofstream sf("stats.txt");
    if (!sf.is_open()) return;
    sf << fixed << setprecision(2)
       << max(0.0, totalRam     - usedRam)     << "\n"
       << max(0.0, totalStorage - usedStorage) << "\n"
       << max(0,   totalCores   - usedCores)   << "\n";
}

// ── Reap dead children ────────────────────────────────────────────────────────
void reapChildren() {
    int status; pid_t dead;
    while ((dead = waitpid(-1, &status, WNOHANG)) > 0)
        guiProcesses.erase(dead);
}

// ── Launch with RAM guard — returns false if RAM is insufficient ───────────────
bool launchProcess(const string& processName) {
    ResourceCost cost = {0.10, 0.50, 1};
    if (taskResources.count(processName)) cost = taskResources[processName];

    // ── RAM CHECK ─────────────────────────────────────────────────────────────
    if (cost.ram > getAvailableRam()) return false;   // blocked — caller shows warning

    pid_t pid = fork();
    if (pid == 0) {
        string path = "./installed_apps/" + processName;
        execlp("xterm", "xterm", "-e", path.c_str(), (char*)NULL);
        exit(0);
    } else if (pid > 0) {
        guiProcesses[pid] = {nextProcId++, processName, cost.ram, cost.storage, cost.cores};
        updateRegistry();
        updateStats();
    }
    return true;
}

void cleanupChildren() {
    for (auto const& [pid, info] : guiProcesses) kill(pid, SIGKILL);
}

// ─────────────────────────────────────────────────────────────────────────────
int main() {
    sf::RenderWindow window(sf::VideoMode(1100, 800), "Shaheer-Ibrahim-Abdullah OS Desktop");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"))
        cerr << "Error: System font not found.\n";

    // ── Title ──────────────────────────────────────────────────────────────────
    sf::Text titleText("Shaheer-Ibrahim-Abdullah OS Desktop", font, 36);
    titleText.setPosition(300, 20);
    titleText.setFillColor(sf::Color::Cyan);

    // ── Resource panel ─────────────────────────────────────────────────────────
    sf::RectangleShape resourcePanel(sf::Vector2f(1000, 120));
    resourcePanel.setPosition(50, 75);
    resourcePanel.setFillColor(sf::Color(40, 40, 40));
    resourcePanel.setOutlineThickness(2);
    resourcePanel.setOutlineColor(sf::Color(100, 100, 100));

    sf::Text ramText("Available RAM: 4.00 GB", font, 20);
    ramText.setPosition(100, 95);
    ramText.setFillColor(sf::Color::Green);

    sf::Text storageText("Storage: 256.00 GB", font, 20);
    storageText.setPosition(450, 95);
    storageText.setFillColor(sf::Color::Green);

    sf::Text coreText("Cores: 4 Free", font, 20);
    coreText.setPosition(800, 95);
    coreText.setFillColor(sf::Color::Green);

    sf::Text procCountText("Active Processes: 0", font, 16);
    procCountText.setPosition(100, 140);
    procCountText.setFillColor(sf::Color::Yellow);

    // ── RAM-full warning banner ────────────────────────────────────────────────
    sf::RectangleShape warnBox(sf::Vector2f(700, 50));
    warnBox.setFillColor(sf::Color(180, 0, 0, 220));
    warnBox.setOutlineThickness(2);
    warnBox.setOutlineColor(sf::Color::Red);

    sf::Text warnText("", font, 20);
    warnText.setFillColor(sf::Color::White);

    bool     showWarning  = false;
    sf::Clock warnClock;

    auto showRamWarning = [&](const string& msg) {
        warnText.setString(msg);
        // Center the box on screen
        warnBox.setPosition(200, 355);
        sf::FloatRect wb = warnBox.getGlobalBounds();
        sf::FloatRect tb = warnText.getLocalBounds();
        warnText.setPosition(wb.left + (wb.width - tb.width) / 2.f - tb.left,
                             wb.top  + (wb.height- tb.height)/ 2.f - tb.top);
        showWarning = true;
        warnClock.restart();
    };

    // ── App list (logGenerator removed) ───────────────────────────────────────
    vector<pair<string,string>> apps = {
        {"calculator","Calculator"},       {"notepad","Notepad"},
        {"music","Music Player"},          {"monitor","Sys Monitor"},
        {"clock","System Clock"},          {"calendar","Calendar"},
        {"timer","Timer"},                 {"fileEditor","File Editor"},
        {"textSearch","Text Search"},      {"fileCopy","File Copy"},
        {"fileMove","File Move"},           {"fileDelete","File Delete"},
        {"fileCreation","Create File"},    {"autoBackup","Backup"},
        {"randomNumberGenerator","Random Numbers"},
        {"ramUsageViewer","RAM Usage"},    {"runningProcessesViewer","Process Info"},
        {"fileInformation","File Info"},
    };

    // ── Build buttons ──────────────────────────────────────────────────────────
    vector<pair<sf::RectangleShape, sf::Text>> buttons;
    int cols = 4, row = 0, col = 0;
    for (const auto& app : apps) {
        sf::RectangleShape shape(sf::Vector2f(220, 85));
        shape.setPosition(60 + col * 250, 215 + row * 115);
        shape.setFillColor((row == 0 && col < 4) ? sf::Color(0, 102, 204) : sf::Color(60, 60, 60));
        shape.setOutlineThickness(2);
        shape.setOutlineColor(sf::Color::White);

        sf::Text text(app.second, font, 16);
        text.setFillColor(sf::Color::White);
        sf::FloatRect r = text.getLocalBounds();
        text.setOrigin(r.left + r.width / 2.f, r.top + r.height / 2.f);
        text.setPosition(shape.getPosition().x + 110.f, shape.getPosition().y + 42.5f);
        buttons.push_back({shape, text});
        if (++col >= cols) { col = 0; ++row; }
    }

    // ── Taskbar ────────────────────────────────────────────────────────────────
    sf::RectangleShape taskbar(sf::Vector2f(1100, 50));
    taskbar.setPosition(0, 750);
    taskbar.setFillColor(sf::Color(20, 20, 20));

    sf::Text statusText("Mode: ADMIN", font, 16);
    statusText.setPosition(30, 765);
    statusText.setFillColor(sf::Color::White);

    sf::Clock updateClock;

    // Seed totals from stats.txt only if they are valid (> 0)
    {
        ifstream sf2("stats.txt");
        double r, s; int c;
        if (sf2 >> r >> s >> c) { 
            if (r > 0) totalRam = r; 
            if (s > 0) totalStorage = s; 
            if (c > 0) totalCores = c; 
        }
    }

    // ── Main loop ──────────────────────────────────────────────────────────────
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                cleanupChildren();
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left)
            {
                sf::Vector2f mp((float)event.mouseButton.x, (float)event.mouseButton.y);
                for (size_t i = 0; i < buttons.size(); ++i) {
                    if (buttons[i].first.getGlobalBounds().contains(mp)) {
                        buttons[i].first.setFillColor(sf::Color::Yellow);

                        bool launched = launchProcess(apps[i].first);
                        if (!launched) {
                            // RAM too full — flash warning
                            ostringstream oss;
                            oss << fixed << setprecision(2)
                                << "  RAM FULL!  Available: " << getAvailableRam()
                                << " GB  — Close a process first.";
                            showRamWarning(oss.str());
                        }
                    }
                }
            }

            if (event.type == sf::Event::MouseButtonReleased) {
                for (size_t i = 0; i < buttons.size(); ++i)
                    buttons[i].first.setFillColor(
                        (i < 4) ? sf::Color(0, 102, 204) : sf::Color(60, 60, 60));
            }
        }

        // ── 1-second refresh ──────────────────────────────────────────────────
        if (updateClock.getElapsedTime().asSeconds() > 1.0f) {
            reapChildren();
            updateRegistry();
            updateStats();

            ifstream statsFile("stats.txt");
            string ramStr, storageStr, coresStr;
            if (statsFile >> ramStr >> storageStr >> coresStr) {
                ramText.setString("Available RAM: " + ramStr + " GB");
                storageText.setString("Storage: " + storageStr + " GB");
                coreText.setString("Cores: " + coresStr + " Free");

                // Tint RAM label red when low (< 0.5 GB)
                double avail = stod(ramStr);
                ramText.setFillColor(avail < 0.5 ? sf::Color::Red : sf::Color::Green);
            }
            procCountText.setString("Active Processes: " + to_string(guiProcesses.size()));
            updateClock.restart();
        }

        // Auto-hide warning after 3 seconds
        if (showWarning && warnClock.getElapsedTime().asSeconds() > 3.0f)
            showWarning = false;

        // ── Draw ──────────────────────────────────────────────────────────────
        window.clear(sf::Color(10, 10, 25));
        window.draw(titleText);
        window.draw(resourcePanel);
        window.draw(ramText);
        window.draw(storageText);
        window.draw(coreText);
        window.draw(procCountText);
        window.draw(taskbar);
        window.draw(statusText);
        for (auto& [shape, text] : buttons) {
            window.draw(shape);
            window.draw(text);
        }
        if (showWarning) {
            window.draw(warnBox);
            window.draw(warnText);
        }
        window.display();
    }
    return 0;
}
