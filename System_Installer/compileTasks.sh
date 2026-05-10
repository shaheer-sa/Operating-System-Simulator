#!/bin/bash

mkdir -p installed_apps

echo "Compiling OS kernel..."
g++ -std=c++17 ccpps/main.cpp ccpps/Kernel.cpp ccpps/PCB.cpp ccpps/ResourceManager.cpp \
    ccpps/Scheduler.cpp ccpps/TaskManager.cpp -I headerfiles -o osSimulator -pthread
if [ $? -eq 0 ]; then echo "osSimulator compiled successfully."; else echo "Error: osSimulator failed to compile."; fi

echo "Compiling core apps..."
for app in calculator notepad music monitor; do
    echo "Compiling $app..."
    g++ -std=c++17 "apps/${app}.cpp" -o "installed_apps/${app}" -pthread
done

echo "Compiling utility tasks..."
for file in tasks/*.cpp; do
    [ -f "$file" ] || continue
    filename=$(basename -- "$file")
    taskname="${filename%.*}"
    echo "Compiling $taskname..."
    g++ -std=c++17 "$file" -o "installed_apps/$taskname" -pthread
done

echo "Compiling Desktop GUI..."
g++ -std=c++17 apps/desktopGUI.cpp -o desktopGUI -lsfml-graphics -lsfml-window -lsfml-system
if [ $? -eq 0 ]; then echo "desktopGUI compiled successfully."; else echo "Warning: desktopGUI failed (SFML may not be installed)."; fi

echo "All components compiled successfully into the installed_apps/ directory."
