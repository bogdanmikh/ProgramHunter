//
// Created by timur on 04.01.2025.
//

#include "Server.hpp"

void Server::onAttach(const ServerCreateInfo& serverCreateInfo) {
    if (enet_initialize() < 0) {
        printf("An error occurred while initializing ENet.\n");
        return;
    }

    ENetAddress address = {0};

    address.host = ENET_HOST_ANY;
    address.port = serverCreateInfo.port;

    auto ip = getLocalIPAddresses()[1];

    /* create a server */
    m_server = enet_host_create(&address, maxClients, 2, 0, 0);

    if (m_server == nullptr) {
        printf("An error occurred while trying to create an ENet server host.\n");
        return;
    }
    printf("Your IP: %s\n", ip.c_str());

    printf("Started a server...\n");
}

void Server::onUpdate(ClientFunc func) {
    ENetEvent event;
    while (enet_host_service(m_server, &event, 10) > 0) {
        if (event.type == ENET_EVENT_TYPE_CONNECT) {
            m_clients.emplace_back(event.peer);
            printf("A new client connected from %x:%u.\n", event.peer->address.host, event.peer->address.port);
        } else if (event.type == ENET_EVENT_TYPE_RECEIVE) {
            auto data = (PushData*)event.packet->data;
            func(*data);
            enet_packet_destroy(event.packet);
        } else if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
            for (int i = 0; i < m_clients.size(); ++i) {
                if (event.peer == m_clients[i]) {
                    m_clients.erase(m_clients.begin() + i);
                    break;
                }
            }
            event.peer->data = nullptr;
        } else if (event.type == ENET_EVENT_TYPE_DISCONNECT_TIMEOUT) {
            printf("%s disconnected due to timeout.\n", event.peer->data);
            /* Reset the peer's client information. */
            event.peer->data = nullptr;
        } else if (event.type == ENET_EVENT_TYPE_NONE) {
            /// ...
        }
    }
    // std::cin >> m_data.message;
    // for (auto client : m_clients) {
    //     sendData((void*)&m_data, sizeof(m_data), client);
    // }
}

void Server::onDetach() {
    enet_host_destroy(m_server);
    enet_deinitialize();
}

void Server::sendData(const void* data, size_t size, ENetPeer *client) {
    ENetPacket *packet = enet_packet_create(data, size, ENET_PACKET_FLAG_RELIABLE);

    enet_peer_send(client, 0, packet);
}