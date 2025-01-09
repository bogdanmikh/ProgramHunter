//
// Created by timur on 04.01.2025.
//

#include "Hunter.hpp"
#include "Config.hpp"

void Hunter::onAttach(const ServerData& serverData, const char* name) {
    if (enet_initialize() < 0) {
        fprintf(stderr, "An error occurred while initializing ENet.\n");
        return;
    }

    m_serverData = serverData;

    m_client = enet_host_create(nullptr /* create a client host */,
                                1 /* only allow 1 outgoing connection */,
                                2 /* allow up 2 channels to be used, 0 and 1 */,
                                0 /* assume any amount of incoming bandwidth */,
                                0 /* assume any amount of outgoing bandwidth */);

    if (m_client == nullptr) {
        fprintf(stderr,
                "An error occurred while trying to create an ENet client host.\n");
        exit(EXIT_FAILURE);
    }

    tryConnect();
    if (m_server == nullptr) {
        fprintf(stderr,
                "No available peers for initiating an ENet connection.\n");
        exit(EXIT_FAILURE);
    }
    if (!serverValid()) {
        printf("Not connected!!!\n");
        return;
    }
    m_connected = true;
    printf("Connected to server!\n");
    m_data.initData.emplace();
    auto &initData = m_data.initData.value();
    strcpy(initData.name, name);
    initData.state = InitData::State::VICTIM;
    sendData(m_data);

    m_data.initData.reset();
    m_connected = true;
}

void Hunter::onUpdate() {
    if (!serverValid()) {
        for (int i = 0; i < COUNT_TRY_CONNECT; ++i) {
            printf("Try connect ...\n");
            tryConnect();
            if (serverValid()) {
                m_connected = true;
                printf("Again connected to server!\n");
                return;
            }
            m_connected = false;
        }
        if (!m_connected) {
            printf("Not connected!!!\n");
            return;
        }
    }
    while (enet_host_service(m_client, &m_event, 10) > 0) {
        if (m_event.type == ENET_EVENT_TYPE_CONNECT) {
        } else if (m_event.type == ENET_EVENT_TYPE_RECEIVE) {
            auto data = (PushData *) m_event.packet->data;
            enet_packet_destroy(m_event.packet);
        } else if (m_event.type == ENET_EVENT_TYPE_DISCONNECT) {
            m_connected = false;
            printf("Disconnection succeeded.");
        }
    }
}

void Hunter::onDetach() {
    enet_peer_reset(m_server);
    enet_host_destroy(m_client);
    enet_deinitialize();
}

bool Hunter::serverValid() {
    if (m_server == nullptr) {
        return false;
    }
    return m_server->state == ENET_PEER_STATE_CONNECTED;
}

void Hunter::sendData(const PushData &pushData) {
    if (!serverValid()) {
        printf("Not connected!!!\n");
        return;
    }
    m_data = pushData;
    sendData((void*)&m_data, sizeof(m_data));
}

void Hunter::sendData(const void* data, size_t size) {
    ENetPacket *packet = enet_packet_create(data, size, ENET_PACKET_FLAG_RELIABLE);
    //the second parameter is the channel id
    enet_peer_send(m_server, 0, packet);
}

bool Hunter::isConnected() const {
    return m_connected;
}

void Hunter::tryConnect() {
    ENetAddress address = { 0 };
    address.host = ENET_HOST_ANY;
    address.port = m_serverData.port;

    enet_address_set_host(&address, m_serverData.ip.c_str());
    enet_address_set_host(&address, m_serverData.ip.c_str());
    address.port = m_serverData.port;
    m_server = enet_host_connect(m_client, &address, 2, 0);
    enet_host_service(m_client, &m_event, TIMEOUT_SERVER);
}