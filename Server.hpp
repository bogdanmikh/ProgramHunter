//
// Created by timur on 04.01.2025.
//
#pragma once

#include "BaseNetwork.hpp"
#include <enet.h>
#include <vector>

struct ServerCreateInfo {
    std::string name;
    int port;
};

using ClientFunc = void (*)(const PushData&);

// у кого что то вырубают
class Server {
public:
    void onAttach(const ServerCreateInfo& serverCreateInfo);
    void onUpdate(ClientFunc func);
    void onDetach();
private:
    static const int maxClients = 4;
    void sendData(const void* data, size_t size, ENetPeer *client);
    ENetHost* m_server;
    std::vector<ENetPeer*> m_clients;
};
