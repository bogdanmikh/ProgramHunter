//
// Created by timur on 04.01.2025.
//

#include <iostream>
#include <cstring>
#include <cstdlib>
#include "PlatformDetection.hpp"

#ifdef PLATFORM_LINUX
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>
#include <csignal>
#include <array>
#include <sstream>
#elif defined(PLATFORM_MACOS)
#include <libproc.h>
#include <vector>
#include <cstring>
#include <csignal>

std::string getProcessName(pid_t pid) {
    char name[1024];
    proc_name(pid, name, sizeof(name));
    return std::string(name);
}
#elif defined(PLATFORM_WINDOWS)
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#endif

#include "ProgramHunter.hpp"

namespace ProgramHunter {
Pid getPidProcess(const char *nameProgram) {
#ifdef PLATFORM_LINUX
    DIR* proc_dir = opendir("/proc");

    if (!proc_dir) {
        std::cerr << "Not open catalog /proc" << std::endl;
        return INVALID_PID;
    }
    struct dirent* entry;
    while ((entry = readdir(proc_dir)) != nullptr) {
        // Проверяем, является ли имя записи числом (PID процесса)
        if (entry->d_type == DT_DIR) {
            int currentPid = atoi(entry->d_name);
            if (currentPid > 0) {
                // Имя процесса можно получить из /proc/[pid]/comm
                std::string commPath = std::string("/proc/") + entry->d_name + "/comm";
                FILE* commFile = fopen(commPath.c_str(), "r");
                if (commFile) {
                    char comm[256];
                    fgets(comm, sizeof(comm), commFile);
                    // std::cout << "PID: " << currentPid << ", Command: " << comm;
                    fclose(commFile);
                    std::string s = std::string(comm);
                    s.pop_back();
                    if (s == std::string(nameProgram)) {
                        return currentPid;
                    }
                }
            }
        }
    }
    closedir(proc_dir);
    return INVALID_PID;
#elif defined(PLATFORM_MACOS)
    pid_t pids[1024];
    int numPids = proc_listallpids(pids, sizeof(pids) / sizeof(pids[0]));

    if (numPids < 0) {
        std::cerr << "Error" << std::endl;
        return INVALID_PID;
    }

    for (int i = 0; i < numPids; ++i) {
        if (pids[i] != 0) {
            std::string processName = getProcessName(pids[i]);
            if (nameProgram == processName) {
                return pids[i];
            }
        }
    }
    return INVALID_PID;
#elif defined(PLATFORM_WINDOWS)
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (snapshot == INVALID_HANDLE_VALUE) {
        return INVALID_PID;
    }

    if (Process32First(snapshot, &entry)) {
        do {
            if (_stricmp(entry.szExeFile, nameProgram) == 0) {
                CloseHandle(snapshot);
                return static_cast<Pid>(entry.th32ProcessID);
            }
        } while (Process32Next(snapshot, &entry));
    }

    CloseHandle(snapshot);
    return INVALID_PID;
#endif
}

void printALlProcess() {
#ifdef PLATFORM_LINUX
    Pid pid = INVALID_PID;
    DIR* proc_dir = opendir("/proc");

    if (!proc_dir) {
        std::cerr << "Not open catalog /proc" << std::endl;
        return;
    }
    struct dirent* entry;
    while ((entry = readdir(proc_dir)) != nullptr) {
        // Проверяем, является ли имя записи числом (PID процесса)
        if (entry->d_type == DT_DIR) {
            int currentPid = atoi(entry->d_name);
            if (currentPid > 0) {
                // Имя процесса можно получить из /proc/[pid]/comm
                std::string commPath = std::string("/proc/") + entry->d_name + "/comm";
                FILE* commFile = fopen(commPath.c_str(), "r");
                if (commFile) {
                    char comm[256];
                    fgets(comm, sizeof(comm), commFile);
                    std::cout << "PID: " << currentPid << ", Command: " << comm;
                    fclose(commFile);
                }
            }
        }
    }
    closedir(proc_dir);
#elif defined(PLATFORM_MACOS)
    pid_t pids[1024];
    int numPids = proc_listallpids(pids, sizeof(pids) / sizeof(pids[0]));

    if (numPids < 0) {
        std::cerr << "Error" << std::endl;
        return;
    }

    std::cout << "PID\tProcess Name" << std::endl;
    std::cout << "-----------------------------" << std::endl;

    for (int i = 0; i < numPids; ++i) {
        if (pids[i] != 0) {
            std::string processName = getProcessName(pids[i]);
            std::cout << pids[i] << "\t" << processName << std::endl;
        }
    }
#elif defined(PLATFORM_WINDOWS)
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        std::cerr << "Error: failed to create a snapshot of processes" << std::endl;
        return;
    }

    if (!Process32First(hProcessSnap, &pe32)) {
        std::cerr << "Error: couldn't get information about the process" << std::endl;
        CloseHandle(hProcessSnap);
        return;
    }

    std::cout << "List of running processes:" << std::endl;

    do {
        std::wcout << L"Pid: " << pe32.th32ProcessID << L", Process: " << pe32.szExeFile << std::endl;
    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);
#endif
}

bool killProcess(Pid pid) {
    if (pid == INVALID_PID) {
        return false;
    }
#ifdef PLATFORM_POSIX
    return kill((pid_t)pid, SIGKILL) == 0;
#elif defined(PLATFORM_WINDOWS)
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, static_cast<DWORD>(pid));

    if (hProcess == NULL) {
        return false;
    }

    BOOL result = TerminateProcess(hProcess, 1);

    CloseHandle(hProcess);

    return result != FALSE;
#endif
}

}