#include "Network/Hunter.hpp"
#include "Network/Victim.hpp"
#include "Network/Server.hpp"
#include "ProgramHunter.hpp"
#include <fstream>
#include <memory>

void victimFunc(void *killProcess) {
    auto nameProcess = (char*)killProcess;
    ProgramHunter::killProcess(ProgramHunter::getPidProcess(nameProcess));
}

int main() {
    std::ifstream inputFile("config.txt");

    std::unique_ptr<Server> server;
    std::unique_ptr<Hunter> hunter;
    std::unique_ptr<Victim> victim;

    int state;
    const int port = 7777;
//        std::cout << "1 is server, 2 is victim, 3 is hunter!!!\n";
    inputFile >> state;
    if (state == 1) {
        server = std::make_unique<Server>();
        server->onAttach({"Server", port});
    } else if (state == 2) {
        victim = std::make_unique<Victim>();
        std::string ipServer, name;
        inputFile >> ipServer;
        inputFile >> name;
        victim->onAttach({ipServer, port}, name.c_str());
    } else if (state == 3) {
        hunter = std::make_unique<Hunter>();
        std::string ipServer, name;
        inputFile >> ipServer;
        inputFile >> name;
        hunter->onAttach({ipServer, port}, name.c_str());
    } else {
        std::cout << "Not Valid value\n";
        return 0;
    }

    while (true) {
        if (state == 1) {
            server->onUpdate();
        } else if (state == 2) {
            victim->onUpdate(victimFunc);
            if (!victim->isConnected()) {
                break;
            }
        } else if (state == 3) {
            if (!hunter->isConnected()) {
                break;
            }
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
    } else if (state == 2) {
        victim->onDetach();
    } else {
        hunter->onDetach();
    }
}