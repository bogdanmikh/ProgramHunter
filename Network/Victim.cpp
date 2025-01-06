//
// Created by Bogdan on 04.01.2025.
//

#include "Victim.hpp"

#include <iostream>

void Victim::onAttach(const ServerData& serverData, const char* name) {
    if (enet_initialize() < 0) {
        fprintf(stderr, "An error occurred while initializing ENet.\n");
        return;
    }

    ENetAddress address = { 0 };
    address.host = ENET_HOST_ANY;
    address.port = serverData.port;

    m_client = enet_host_create(nullptr /* create a client host */,
                                1 /* only allow 1 outgoing connection */,
                                2 /* allow up 2 channels to be used, 0 and 1 */,
                                0 /* assume any amount of incoming bandwidth */,
                                0 /* assume any amount of outgoing bandwidth */);

    enet_address_set_host(&address, serverData.ip.c_str());
    address.port = serverData.port;
    m_server = enet_host_connect(m_client, &address, 2, 0);

    // m_client = nullptr;
    if (m_client == nullptr) {
        fprintf(stderr,
                "An error occurred while trying to create an ENet client host.\n");
        exit(EXIT_FAILURE);
    }

    if (m_server == nullptr) {
        fprintf(stderr,
                "No available peers for initiating an ENet connection.\n");
        exit(EXIT_FAILURE);
    }
    while (!m_connected) {
        m_connected = serverValid();
    }
    std::cout << "Connected to server!" << std::endl;
    m_data.initData.emplace();
    auto &initData = m_data.initData.value();
    strcpy(initData.name, name);
    initData.state = InitData::State::VICTIM;
    sendData(m_data);
    m_data.initData.reset();
}

void Victim::onUpdate(VictimFunc func) {
    while (enet_host_service(m_client, &m_event, 10) > 0) {
        if (m_event.type == ENET_EVENT_TYPE_CONNECT) {
            m_connected = true;
            std::cout << "Connected to server!" << std::endl;
            sendData(m_data);
        } else if (m_event.type == ENET_EVENT_TYPE_RECEIVE) {
            auto data = m_event.packet->data;
            func(data);
            enet_packet_destroy(m_event.packet);
            // sendData((void*)&m_data, sizeof(m_data));
        } else if (m_event.type == ENET_EVENT_TYPE_DISCONNECT) {
            puts("Disconnection succeeded.");
            m_connected = false;
        }
    }

//    usleep(16000);
}

void Victim::onDetach() {
    if (!m_connected) {
        enet_peer_reset(m_server);
    }
    enet_host_destroy(m_client);
    enet_deinitialize();
}

bool Victim::serverValid() {
    if (m_connected) {
        return true;
    }
    static int timeoutServer = 100;
    return enet_host_service(m_client, &m_event, timeoutServer ) > 0;
}

void Victim::sendData(const PushData &pushData) {
    if (m_server->state != ENET_PEER_STATE_CONNECTED) {
        return;
    }
    m_data = pushData;
    sendData((void*)&m_data, sizeof(m_data));
}

void Victim::sendData(const void* data, size_t size) {
    ENetPacket *packet = enet_packet_create(data, size, ENET_PACKET_FLAG_RELIABLE);
    //the second parameter is the channel id
    enet_peer_send(m_server, 0, packet);
}