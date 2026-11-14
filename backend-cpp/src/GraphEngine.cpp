#include "../include/GraphEngine.hpp"
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <iomanip>

bool GraphEngine::edgeExists(int u, int v) const {
    if (u < 0 || u >= static_cast<int>(adjList.size())) return false;
    for (int neighbor : adjList[u]) {
        if (neighbor == v) return true;
    }
    return false;
}

int GraphEngine::addUser(const std::string& name, const std::string& handle) {
    int newId = static_cast<int>(users.size());
    users.push_back({newId, name, handle});
    adjList.emplace_back();
    return newId;
}

bool GraphEngine::addFriendship(int u, int v) {
    int n = static_cast<int>(users.size());
    if (u < 0 || u >= n || v < 0 || v >= n || u == v) return false;
    if (edgeExists(u, v)) return false;

    adjList[u].push_back(v);
    adjList[v].push_back(u);
    return true;
}

std::vector<Recommendation> GraphEngine::getRecommendations(int targetId, int topK) const {
    int n = static_cast<int>(users.size());
    if (targetId < 0 || targetId >= n) return {};

    // On-Demand O(V) Temporary Allocations
    std::vector<int> mutualCount(n, 0);
    std::vector<double> adamicAdar(n, 0.0);
    std::vector<bool> isExcluded(n, false);

    // Exclude target user and existing Level-1 direct friends
    isExcluded[targetId] = true;
    for (int directFriend : adjList[targetId]) {
        isExcluded[directFriend] = true;
    }

    // Distance-2 Traversal
    for (int directFriend : adjList[targetId]) {
        int friendDegree = static_cast<int>(adjList[directFriend].size());
        // Dampening weight for popular nodes: 1 / log(degree)
        double weight = (friendDegree > 1) ? (1.0 / std::log(friendDegree)) : 1.0;

        for (int candidate : adjList[directFriend]) {
            if (!isExcluded[candidate]) {
                mutualCount[candidate]++;
                adamicAdar[candidate] += weight;
            }
        }
    }

    // Collect valid candidates
    std::vector<Recommendation> results;
    for (int i = 0; i < n; ++i) {
        if (mutualCount[i] > 0) {
            results.push_back({i, mutualCount[i], adamicAdar[i]});
        }
    }

    // Sort primarily by Mutual Friends, then by Adamic-Adar score
    std::sort(results.begin(), results.end(), [](const Recommendation& a, const Recommendation& b) {
        if (a.mutualCount != b.mutualCount) {
            return a.mutualCount > b.mutualCount;
        }
        return a.adamicAdarScore > b.adamicAdarScore;
    });

    if (static_cast<int>(results.size()) > topK) {
        results.resize(topK);
    }

    return results;
}

bool GraphEngine::saveToFile(const std::string& filename) const {
    std::ofstream out(filename);
    if (!out.is_open()) return false;

    out << users.size() << "\n";
    for (const auto& u : users) {
        out << u.id << "|" << u.name << "|" << u.handle << "\n";
    }

    for (size_t u = 0; u < adjList.size(); ++u) {
        for (int v : adjList[u]) {
            if (static_cast<int>(u) < v) {
                out << u << " " << v << "\n";
            }
        }
    }
    return true;
}

bool GraphEngine::loadFromFile(const std::string& filename) {
    std::ifstream in(filename);
    if (!in.is_open()) return false;

    users.clear();
    adjList.clear();

    int n = 0;
    if (!(in >> n)) return false;
    std::string dummy;
    std::getline(in, dummy);

    users.resize(n);
    adjList.resize(n);

    for (int i = 0; i < n; ++i) {
        std::string line;
        if (!std::getline(in, line)) return false;
        std::stringstream ss(line);
        std::string idStr, name, handle;
        
        std::getline(ss, idStr, '|');
        std::getline(ss, name, '|');
        std::getline(ss, handle, '|');

        int id = std::stoi(idStr);
        users[id] = {id, name, handle};
    }

    int u, v;
    while (in >> u >> v) {
        addFriendship(u, v);
    }
    return true;
}

void GraphEngine::seedDefaultData() {
    users.clear();
    adjList.clear();

    addUser("Ojas Dongre", "ojas");
    addUser("Prajwal Patekar", "prajwal");
    addUser("Ved Niturkar", "ved");
    addUser("Raj Shinde", "raj");
    addUser("Prathamesh Kulkarni", "prathamesh");
    addUser("Chinmay Mudgalkar", "chinmay");
    addUser("Atharva Joshi", "atharva");

    addFriendship(0, 1);
    addFriendship(0, 2);
    addFriendship(0, 4);
    addFriendship(1, 3);
    addFriendship(1, 5);
    addFriendship(2, 3);
    addFriendship(2, 6);
    addFriendship(4, 3);
}

std::string GraphEngine::exportStateToJson(int activeUserId) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    oss << "{\n";

    // Users array
    oss << "  \"users\": [\n";
    for (size_t i = 0; i < users.size(); ++i) {
        oss << "    {\"id\": " << users[i].id 
            << ", \"name\": \"" << users[i].name 
            << "\", \"handle\": \"" << users[i].handle 
            << "\", \"degree\": " << adjList[i].size() << "}"
            << (i + 1 < users.size() ? "," : "") << "\n";
    }
    oss << "  ],\n";

    // Direct friends of activeUserId
    oss << "  \"directFriends\": [";
    if (activeUserId >= 0 && activeUserId < static_cast<int>(adjList.size())) {
        const auto& friends = adjList[activeUserId];
        for (size_t i = 0; i < friends.size(); ++i) {
            oss << friends[i] << (i + 1 < friends.size() ? ", " : "");
        }
    }
    oss << "],\n";

    // Edges list
    oss << "  \"edges\": [\n";
    bool firstEdge = true;
    for (size_t u = 0; u < adjList.size(); ++u) {
        for (int v : adjList[u]) {
            if (static_cast<int>(u) < v) {
                if (!firstEdge) oss << ",\n";
                oss << "    {\"source\": " << u << ", \"target\": " << v << "}";
                firstEdge = false;
            }
        }
    }
    oss << "\n  ],\n";

    // Recommendations for activeUserId
    auto recs = getRecommendations(activeUserId, 10);
    oss << "  \"recommendations\": [\n";
    for (size_t i = 0; i < recs.size(); ++i) {
        const auto& r = recs[i];
        oss << "    {\"id\": " << r.userId
            << ", \"name\": \"" << users[r.userId].name
            << "\", \"handle\": \"" << users[r.userId].handle
            << "\", \"mutualCount\": " << r.mutualCount
            << ", \"adamicAdar\": " << r.adamicAdarScore << "}"
            << (i + 1 < recs.size() ? "," : "") << "\n";
    }
    oss << "  ]\n";
    oss << "}";

    return oss.str();
}
