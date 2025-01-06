//
// Created by timur on 04.01.2025.
//
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include "PlatformDetection.hpp"

#ifdef PLATFORM_WINDOWS
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <cstring>
#endif


static std::vector<std::string> getLocalIPAddresses() {
    std::vector<std::string> ipAddresses;

#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    char hostname[256];
    gethostname(hostname, sizeof(hostname));

    struct hostent *host = gethostbyname(hostname);
    if (host) {
        for (int i = 0; host->h_addr_list[i] != nullptr; ++i) {
            struct in_addr addr;
            memcpy(&addr, host->h_addr_list[i], sizeof(struct in_addr));
            ipAddresses.push_back(inet_ntoa(addr));
        }
    }

    WSACleanup();
#else
    struct ifaddrs *ifaddr;
    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return ipAddresses;
    }

    for (struct ifaddrs *ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET) {
            char ip[INET_ADDRSTRLEN];
            if (inet_ntop(AF_INET, &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr, ip, sizeof(ip))) {
                ipAddresses.push_back(ip);
            }
        }
    }
    freeifaddrs(ifaddr);
#endif

    return ipAddresses;
}

struct HunterData {
    char nameProcess[100];
    char nameVictim[100];
};

struct InitData {
    enum State { UNKNOWN = 0, VICTIM, HUNTER };
    State state;
    char name[100];
};

struct PushData {
    std::optional<InitData> initData;
    std::optional<HunterData> hunter;
};


struct ServerCreateInfo {
    std::string name;
    int port;
};

struct ServerData {
    // localhost 127.0.0.1
    std::string ip;
    int port;
};
