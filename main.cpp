#include "Network/Hunter.hpp"
#include "Network/Victim.hpp"
#include "Network/Server.hpp"
#include "ProgramHunter.hpp"
#include <fstream>

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
    std::ifstream inputFile("config.txt");

    Server* server = nullptr;
    Hunter* hunter = nullptr;
    Victim* victim = nullptr;

    int state;
    const int port = 7777;
    while (true) {
//        std::cout << "1 is server, 2 is victim, 3 is hunter!!!\n";
        inputFile >> state;
        if (state == 1) {
            server = new Server;
            server->onAttach({"Server", port});
            break;
        } else if (state == 2) {
            victim = new Victim;
            std::string ipServer, name;
            inputFile >> ipServer;
            inputFile >> name;
//            ipServer = "127.0.0.1";
            victim->onAttach({ipServer, port}, name.c_str());
            break;
        } else if (state == 3) {
            hunter = new Hunter;
            std::string ipServer, name;
            inputFile >> ipServer;
            inputFile >> name;
//            ipServer = "127.0.0.1";
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
            std::cout << "Enter the victim's name: ";
            std::string nameVictim, processName;
            std::cin >> nameVictim;
            std::cout << "Enter the process name: ";
            std::cin >> processName;
            PushData pushData;
            pushData.hunter.emplace();
            auto &hunterData = pushData.hunter.value();
            strcpy(hunterData.nameVictim, nameVictim.c_str());
            strcpy(hunterData.nameProcess, processName.c_str());
            hunter->sendData(pushData);
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