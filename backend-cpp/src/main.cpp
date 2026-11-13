#include "../include/GraphEngine.hpp"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    const std::string dataFile = "network_data.txt";
    GraphEngine engine;

    if (!engine.loadFromFile(dataFile)) {
        engine.seedDefaultData();
        engine.saveToFile(dataFile);
    }

    std::string command = (argc > 1) ? argv[1] : "state";

    if (command == "state") {
        int activeUser = (argc > 2) ? std::stoi(argv[2]) : 0;
        std::cout << engine.exportStateToJson(activeUser) << std::endl;
    } 
    else if (command == "add_user" && argc >= 4) {
        int newId = engine.addUser(argv[2], argv[3]);
        engine.saveToFile(dataFile);
        std::cout << engine.exportStateToJson(newId) << std::endl;
    } 
    else if (command == "add_edge" && argc >= 4) {
        int u = std::stoi(argv[2]);
        int v = std::stoi(argv[3]);
        engine.addFriendship(u, v);
        engine.saveToFile(dataFile);
        std::cout << engine.exportStateToJson(u) << std::endl;
    } 
    else if (command == "reset") {
        engine.seedDefaultData();
        engine.saveToFile(dataFile);
        std::cout << engine.exportStateToJson(0) << std::endl;
    }

    return 0;
}
