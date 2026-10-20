#pragma once
#include <algorithm>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include "AVLTree.hpp"

// Undirected friendship graph stored as an adjacency list.
// Each user is a vertex number; names map to numbers with an unordered_map.
class SocialNet {
    struct User {
        std::string name;
        std::vector<int> friends;              // adjacency list (vertex ids)
        std::unique_ptr<AVLTree> posts = std::make_unique<AVLTree>();
    };
    std::vector<User> users;
    std::unordered_map<std::string, int> idOf;
    long long clock = 0;                       // increases with every post

    bool areFriends(int a, int b) const {
        for (int f : users[a].friends) if (f == b) return true;
        return false;
    }

public:
    // Returns -1 if the user does not exist.
    int find(const std::string& name) const {
        auto it = idOf.find(name);
        return it == idOf.end() ? -1 : it->second;
    }

    bool addUser(const std::string& name) {
        if (find(name) != -1) return false;
        idOf[name] = (int)users.size();
        users.push_back(User{name, {}});
        return true;
    }

    // Returns false if they are already friends.
    bool addFriend(int a, int b) {
        if (areFriends(a, b)) return false;
        users[a].friends.push_back(b);
        users[b].friends.push_back(a);
        return true;
    }

    std::vector<std::string> listFriends(int u) const {
        std::vector<std::string> names;
        for (int f : users[u].friends) names.push_back(users[f].name);
        std::sort(names.begin(), names.end());
        return names;
    }

    // Friends-of-friends who are not u and not already friends,
    // ranked by number of mutual friends (desc), then name (asc).
    std::vector<std::string> suggestFriends(int u, int n) const {
        std::vector<int> mutual(users.size(), 0);
        std::vector<bool> excluded(users.size(), false);
        excluded[u] = true;
        for (int f : users[u].friends) excluded[f] = true;

        std::vector<int> candidates;
        for (int f : users[u].friends)
            for (int fof : users[f].friends)
                if (!excluded[fof]) {
                    if (mutual[fof] == 0) candidates.push_back(fof);
                    mutual[fof]++;          // one more mutual friend (f)
                }

        std::sort(candidates.begin(), candidates.end(), [&](int a, int b) {
            if (mutual[a] != mutual[b]) return mutual[a] > mutual[b];
            return users[a].name < users[b].name;
        });
        std::vector<std::string> out;
        for (int i = 0; i < (int)candidates.size() && i < n; i++) out.push_back(users[candidates[i]].name);
        return out;
    }

    // Breadth-first search: number of edges on the shortest path, or -1.
    int degreesOfSeparation(int from, int to) const {
        std::vector<int> dist(users.size(), -1);
        std::vector<int> queue{from};          // vector + head index works as a queue
        dist[from] = 0;
        for (size_t head = 0; head < queue.size(); head++) {
            int cur = queue[head];
            if (cur == to) return dist[cur];
            for (int next : users[cur].friends)
                if (dist[next] == -1) {
                    dist[next] = dist[cur] + 1;
                    queue.push_back(next);
                }
        }
        return -1;
    }

    void addPost(int u, const std::string& content) { users[u].posts->add(++clock, content); }

    std::vector<std::string> outputPosts(int u, int n) const { return users[u].posts->recent(n); }
};
