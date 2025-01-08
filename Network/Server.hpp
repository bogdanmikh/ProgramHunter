//
// Created by timur on 04.01.2025.
//
#pragma once

#include "BaseNetwork.hpp"
#include <enet.h>
#include <set>

struct ClientData {
    char name[100];
    InitData::State state;
    ENetPeer* peer;
};

class Server {
public:
    void onAttach(const ServerCreateInfo& serverCreateInfo);
    void onUpdate();
    void onDetach();
private:
    void initClient(const ClientData &clientData);
    int getIndexByPeer(ENetPeer *client);
    int getIndexByName(const char *name);
    static const int maxClients = 4;
    void sendData(const void* data, size_t size, ENetPeer *client);
    ENetHost* m_server;
    std::vector<ClientData> m_clients;
    ServerCreateInfo m_serverInfo;
};
