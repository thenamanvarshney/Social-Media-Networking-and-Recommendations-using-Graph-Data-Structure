#ifndef GRAPH_ENGINE_HPP
#define GRAPH_ENGINE_HPP

#include "User.hpp"
#include <vector>
#include <string>

using namespace std;

class GraphEngine {
private:
    vector<User> users;
    vector<vector<int>> adjList;

    bool edgeExists(int u, int v) const;

public:
    GraphEngine() = default;

    // State Management
    int addUser(const string& name, const string& handle);
    bool deleteUser(int id);
    bool addFriendship(int u, int v);
    
    // Persistence
    bool loadFromFile(const string& filename);
    bool saveToFile(const string& filename) const;
    void seedDefaultData();

    // Core Algorithm: Distance-2 Triadic Closure & Adamic-Adar
    vector<Recommendation> getRecommendations(int targetId, int topK = 5) const;

    // JSON Serialization for API consumption
    string exportStateToJson(int activeUserId) const;
};

#endif // GRAPH_ENGINE_HPP
