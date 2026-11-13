#ifndef USER_HPP
#define USER_HPP

#include <string>

struct User {
    int id;             // Internal contiguous index [0, N-1]
    std::string name;   // Display name
    std::string handle; // Unique username/handle
};

struct Recommendation {
    int userId;
    int mutualCount;
    double adamicAdarScore;
};

#endif // USER_HPP
