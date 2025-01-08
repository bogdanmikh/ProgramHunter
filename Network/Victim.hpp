//
// Created by timur on 04.01.2025.
//
#pragma once

#include "BaseNetwork.hpp"
#include <enet.h>

using VictimFunc = void(*)(void*);

class Victim {
public:
    void onAttach(const ServerData& serverData, const char* name);
    void onUpdate(VictimFunc func);
    void onDetach();
    void sendData(const PushData &pushData);
    bool isConnected() const;
private:
    bool serverValid();
    void tryConnect();
    void sendData(const void* data, size_t size);
    ENetHost *m_client = nullptr;
    ENetPeer* m_server = nullptr;
    ENetEvent m_event;
    ServerData m_serverData;

    PushData m_data;

    bool m_connected = false;
};