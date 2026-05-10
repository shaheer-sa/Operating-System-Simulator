# Operating System Simulator

A production-grade, modular Operating System Simulator built in C++17. This project simulates core kernel functionalities, including multi-level queue scheduling, resource management, and a graphical desktop interface.

## Key Features
- **Multilevel Queue Scheduler**: Implements a sophisticated scheduling system with Round Robin (Level 1) and FCFS (Level 2) logic.
- **Dynamic Resource Management**: Thread-safe tracking of RAM, Storage, and CPU Cores using mutex synchronization.
- **Interactive Desktop GUI**: A bonus SFML-based graphical interface that allows users to launch applications in isolated terminal windows.
- **Robust Task Manager**: Handles process creation (fork), execution (exec), and automatic resource reclamation upon completion.
- **19+ Integrated Applications**: Includes a suite of tools ranging from system utilities (File Copy, RAM Viewer) to productivity apps (Calculator, Notepad).
- **Boot Sequence**: Features a simulated hardware initialization and boot animation for a premium user experience.

## Architecture
The project is divided into several modular components:
- ccpps/: Core Kernel and Manager implementations.
- headerfiles/: Class definitions and system interfaces.
- apps/: Source code for the main GUI and standalone applications.
- tasks/: Source code for modular utility tasks.
- System_Installer/: Build scripts and system maintenance tools.
- installed_apps/: Production-ready binaries (compiled on Linux).

## Getting Started (Linux/Ubuntu)

### Prerequisites
- g++ (C++17 support)
- libsfml-dev (for the Desktop GUI)
- xterm (for application windows)

### Installation & Build
1. Clone the repository:
   ```bash
   git clone https://github.com/shaheer-sa/Operating-System-Simulator.git
   cd Operating-System-Simulator
   ```
2. Build the entire system using the provided installer:
   ```bash
   chmod +x System_Installer/compileTasks.sh
   ./System_Installer/compileTasks.sh
   ```

### Running the Simulator
- **CLI Mode**: ./osSimulator 4 256 8 (Arguments: RAM in GB, Storage in GB, Cores)
- **GUI Mode**: ./desktopGUI

## Commands
- launch [app]: Start a new application.
- listApps: View the catalog of installed software.
- killProcess [id]: Terminate a running task.
- switchMode: Toggle between Administrator and Guest access.

## Authors
Developed by **Shaheer, Ibrahim, and Abdullah** as a Semester Project for Operating Systems.

---
*Note: This simulator is designed to run in a Linux environment with X11 support.*
