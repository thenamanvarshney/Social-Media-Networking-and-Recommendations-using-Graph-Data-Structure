#include "../include/GraphEngine.hpp"
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
    const string dataFile = "network_data.txt";
    GraphEngine engine;

    if (!engine.loadFromFile(dataFile)) {
        engine.seedDefaultData();
        engine.saveToFile(dataFile);
    }

    string command = (argc > 1) ? argv[1] : "state";

    if (command == "state") {
        int activeUser = (argc > 2) ? stoi(argv[2]) : 0;
        cout << engine.exportStateToJson(activeUser) << endl;
    } 
    else if (command == "add_user" && argc >= 4) {
        int newId = engine.addUser(argv[2], argv[3]);
        engine.saveToFile(dataFile);
        cout << engine.exportStateToJson(newId) << endl;
    } 
    else if (command == "delete_user" && argc >= 3) {
        int id = stoi(argv[2]);
        engine.deleteUser(id);
        engine.saveToFile(dataFile);
        cout << engine.exportStateToJson(0) << endl;
    } 
    else if (command == "add_edge" && argc >= 4) {
        int u = stoi(argv[2]);
        int v = stoi(argv[3]);
        engine.addFriendship(u, v);
        engine.saveToFile(dataFile);
        cout << engine.exportStateToJson(u) << endl;
    } 
    else if (command == "reset") {
        engine.seedDefaultData();
        engine.saveToFile(dataFile);
        cout << engine.exportStateToJson(0) << endl;
    }

    return 0;
}
