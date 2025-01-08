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

    m_serverInfo = serverCreateInfo;
}

void Server::onUpdate() {
    ENetEvent event;
    while (enet_host_service(m_server, &event, 10) > 0) {
        if (event.type == ENET_EVENT_TYPE_CONNECT) {
            m_clients.emplace_back();
            m_clients.back().state = InitData::State::UNKNOWN;
            m_clients.back().peer = event.peer;
            printf("A new client connected from %x:%u.\n", event.peer->address.host, event.peer->address.port);
        } else if (event.type == ENET_EVENT_TYPE_RECEIVE) {
            auto data = (PushData*)event.packet->data;
            auto init = data->initData;
            if (init.has_value()) {
                ClientData clientData;
                strcpy(clientData.name, data->initData.value().name);
                clientData.state = data->initData.value().state;
                clientData.peer = event.peer;
                initClient(clientData);
            } else if (data->hunter.has_value()) {
                auto hunterData = data->hunter.value();
                auto victimName = hunterData.nameVictim;
                int index = getIndexByPeer(event.peer);
                if (index == -1) {
                    printf("Hunter not found!\n");
                    return;
                }
                printf("Hunter: %s, Attacked: %s\n", m_clients[getIndexByPeer(event.peer)].name, victimName);
                for (auto &client : m_clients) {
                    if (client.state == InitData::State::UNKNOWN || client.state == InitData::State::HUNTER) {
                        continue;
                    }
                    if (strcmp(client.name, victimName) == 0) {
                        sendData(hunterData.nameProcess, sizeof (hunterData.nameProcess), client.peer);
                        break;
                    }
                }
            }
            enet_packet_destroy(event.packet);
        } else if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
            int index = getIndexByPeer(event.peer);
            printf("Disconnect: %s\n", m_clients[index].name);
            m_clients.erase(m_clients.begin() + index);
            event.peer->data = nullptr;
        } else if (event.type == ENET_EVENT_TYPE_DISCONNECT_TIMEOUT) {
            int index = getIndexByPeer(event.peer);
            printf("Disconnected timeout: %s\n", m_clients[index].name);
            m_clients.erase(m_clients.begin() + index);
            /* Reset the peer's client information. */
            event.peer->data = nullptr;
        } else if (event.type == ENET_EVENT_TYPE_NONE) {
            /// ...
        }
    }
}

void Server::onDetach() {
    enet_host_destroy(m_server);
    enet_deinitialize();
}

void Server::initClient(const ClientData &clientData) {
//    std::cout << "Connected: " << clientData.name << std::endl;
    for (auto &client : m_clients) {
        if (client.state != clientData.state) {
            continue;
        }
        if (strcmp(client.name, clientData.name) == 0) {
            std::cout << "This name is occupied\n";
            return;
        }
    }
    int index = getIndexByPeer(clientData.peer);
    m_clients[index].peer = clientData.peer;
    m_clients[index].state = clientData.state;
    printf("New client: %s\n", clientData.name);
    strcpy(m_clients[index].name, clientData.name);
}

int Server::getIndexByPeer(ENetPeer *client) {
    for (int i = 0; i < m_clients.size(); ++i) {
        if (m_clients[i].peer->connectID == client->connectID) {
            return i;
        }
    }
    return -1;
}

int Server::getIndexByName(const char *name) {
    for (int i = 0; i < m_clients.size(); ++i) {
        if (strcmp(m_clients[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void Server::sendData(const void* data, size_t size, ENetPeer *client) {
    ENetPacket *packet = enet_packet_create(data, size, ENET_PACKET_FLAG_RELIABLE);

    enet_peer_send(client, 0, packet);
}