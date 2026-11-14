#ifndef GRAPH_ENGINE_HPP
#define GRAPH_ENGINE_HPP

#include "User.hpp"
#include <vector>
#include <string>

class GraphEngine {
private:
    std::vector<User> users;
    std::vector<std::vector<int>> adjList;

    bool edgeExists(int u, int v) const;

public:
    GraphEngine() = default;

    // State Management
    int addUser(const std::string& name, const std::string& handle);
    bool deleteUser(int id);
    bool addFriendship(int u, int v);
    
    // Persistence
    bool loadFromFile(const std::string& filename);
    bool saveToFile(const std::string& filename) const;
    void seedDefaultData();

    // Core Algorithm: Distance-2 Triadic Closure & Adamic-Adar
    std::vector<Recommendation> getRecommendations(int targetId, int topK = 5) const;

    // JSON Serialization for API consumption
    std::string exportStateToJson(int activeUserId) const;
};

#endif // GRAPH_ENGINE_HPP
