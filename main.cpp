#include "Client.hpp"
#include "ProgramHunter.hpp"

#include "Server.hpp"
#include "Client.hpp"

void clientFunc(const PushData& data) {
    ProgramHunter p;
    p.killProcess(p.getPidProcess(std::string(data.message).c_str()));
    std::cout << "Message: " << data.message << std::endl;
}

int main() {
    Server* server = nullptr;
    Client* client = nullptr;
    std::cout << "1 is server, 2 is client\n";
    int state;
    const int port = 7777;
    while (true) {
        std::cin >> state;
        if (state == 1) {
            server = new Server;
            server->onAttach({"Server", port});
            break;
        } else if (state == 2) {
            client = new Client;
            std::string ipServer;
            // std::cout << "Input ip server: ";
            // std::cin >> ipServer;
            ipServer = "127.0.0.1";
            client->onAttach({ipServer, port});
            break;
        } else {
            std::cout << "Not Valid value\n";
        }
    }

    for (int i = 0; i < 1000000; ++i) {
        if (state == 1) {
            server->onUpdate(clientFunc);
        } else if (state == 2) {
            client->onUpdate();
            if (!client->serverValid()) {
                break;
            }
        }
    }

    if (state == 1) {
        server->onDetach();
        delete server;
    } else if (state == 2) {
        client->onDetach();
        delete client;
    }
}