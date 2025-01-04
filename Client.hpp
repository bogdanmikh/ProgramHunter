//
// Created by timur on 04.01.2025.
//
#pragma once

#include "BaseNetwork.hpp"
#include <enet.h>
struct ServerData {
    // localhost 127.0.0.1
    std::string ip;
    int port;
};

class Client {
public:
    void onAttach(const ServerData& serverData);
    void onUpdate();
    void onDetach();
    bool serverValid();
    void sendData(const PushData &pushData);
private:
    void sendData(const void* data, size_t size);
    ENetHost *m_client = nullptr;
    ENetPeer* m_server = nullptr;
    ENetEvent m_event;

    PushData m_data;
    bool m_connected = false;
};