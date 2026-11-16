#ifndef USER_HPP
#define USER_HPP

#include <string>

using namespace std;

struct User {
    int id;             // Internal contiguous index [0, N-1]
    string name;        // Display name
    string handle;      // Unique username/handle
};

struct Recommendation {
    int userId;
    int mutualCount;
    double adamicAdarScore;
};

#endif // USER_HPP
