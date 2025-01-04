//
// Created by timur on 04.01.2025.
//

#pragma once

#include <cstdint>

#define INVALID_PID -1

using Pid = int64_t;

class ProgramHunter {
public:
    Pid getPidProcess(const char* nameProgram);
    void printALlProcess();
    bool killProcess(Pid pid);
private:
};