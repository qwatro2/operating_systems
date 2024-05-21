#pragma once
#include <cstdio>
#include <cstdlib>
#include <iostream>

void ExitWithError(const char* message) {
    perror(message);
    exit(1);
}

void PrintHelpServer(const char* execName) {
    std::cout << "\033[31mWrong arguments.\033[0m\nUsage: " << execName << " <port> <numberOfSectors> <treasureIndex>\n";
}

void PrintHelpServerWithMonitor(const char* execName) {
    std::cout << "\033[31mWrong arguments.\033[0m\nUsage: " << execName << " <port> <monitorPort> <numberOfSectors> <treasureIndex>\n";
}

void PrintHelpMonitor(const char* execName) {
    std::cout << "\033[31mWrong arguments.\033[0m\nUsage: " << execName << " <server> <port> <numberOfSectors>\n";
}

void PrintHelpClient(const char* execName) {
    std::cout << "\033[31mWrong arguments.\033[0m\nUsage: " << execName << " <server> <port>\n";
}
