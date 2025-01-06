#include "Network/Hunter.hpp"
#include "Network/Victim.hpp"
#include "Network/Server.hpp"
#include "ProgramHunter.hpp"

void victimFunc(void *killProcess) {
    auto nameProcess = (char*)killProcess;
    ProgramHunter::killProcess(ProgramHunter::getPidProcess(nameProcess));
}

void clientInputFunc(PushData& data) {
    std::cout << "Input process: \n";
    std::string s;
    std::cin >> s;
//    strcpy(data.message, s.c_str());
}

int main() {
    Server* server = nullptr;
    Hunter* hunter = nullptr;
    Victim* victim = nullptr;
    int state;
    const int port = 7777;
    while (true) {
        std::cout << "1 is server, 2 is victim, 3 is hunter!!!\n";
        std::cin >> state;
        if (state == 1) {
            server = new Server;
            server->onAttach({"Server", port});
            break;
        } else if (state == 2) {
            victim = new Victim;
            std::string ipServer, name;
            std::cout << "Input ip server: ";
//            std::cin >> ipServer;
            std::cout << "Input name: ";
            std::cin >> name;
            ipServer = "127.0.0.1";
            victim->onAttach({ipServer, port}, name.c_str());
            break;
        } else if (state == 3) {
            hunter = new Hunter;
            std::string ipServer, name;
            std::cout << "Input ip server: ";
//            std::cin >> ipServer;
            std::cout << "Input name: ";
            std::cin >> name;
            ipServer = "127.0.0.1";
            hunter->onAttach({ipServer, port}, name.c_str());
            break;
        } else {
            std::cout << "Not Valid value\n";
        }
    }

    for (int i = 0; i < 1000000; ++i) {
        if (state == 1) {
            server->onUpdate();
        } else if (state == 2) {
            victim->onUpdate(victimFunc);
        } else if (state == 3) {
            hunter->onUpdate();
            if (i == 0) {
                PushData pushData;
                pushData.hunter.emplace();
                auto &hunterData = pushData.hunter.value();
                strcpy(hunterData.nameVictim, "timur");
                strcpy(hunterData.nameProcess, "Arc");
                hunter->sendData(pushData);
            }
        }
    }

    if (state == 1) {
        server->onDetach();
        delete server;
    } else if (state == 2) {
        victim->onDetach();
        delete victim;
    } else {
        hunter->onDetach();
        delete hunter;
    }
}