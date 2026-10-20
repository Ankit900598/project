#include <cctype>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "SocialNet.hpp"

static std::string lower(std::string s) {
    for (char& c : s) c = (char)std::tolower((unsigned char)c);
    return s;
}

static void error(const std::string& msg) { std::cout << "Error: " << msg << "\n"; }

// Parses a whole-string integer. Returns false if it is not one.
static bool toInt(const std::string& s, int& out) {
    try { size_t used; out = std::stoi(s, &used); return used == s.size(); }
    catch (...) { return false; }
}

static void printList(const std::vector<std::string>& items) {
    for (size_t i = 0; i < items.size(); i++) std::cout << (i ? " " : "") << items[i];
    std::cout << "\n";
}

int main() {
    SocialNet net;
    std::string line;
    while (std::getline(std::cin, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        std::istringstream in(line);
        std::vector<std::string> w;            // all words, lowercased
        for (std::string t; in >> t;) w.push_back(lower(t));
        if (w.empty()) continue;
        std::string cmd = w.size() >= 2 ? w[0] + " " + w[1] : w[0];

        // Look up a user by name, printing an error if missing.
        auto user = [&](const std::string& name) {
            int id = net.find(name);
            if (id == -1) error("user '" + name + "' does not exist");
            return id;
        };

        if (cmd == "add user") {
            if (w.size() != 3) error("usage: ADD USER <username>");
            else if (!net.addUser(w[2])) error("user '" + w[2] + "' already exists");
        } else if (cmd == "add friend") {
            if (w.size() != 4) { error("usage: ADD FRIEND <user1> <user2>"); continue; }
            int a = user(w[2]), b = user(w[3]);
            if (a == -1 || b == -1) continue;
            if (a == b) error("a user cannot befriend themselves");
            else if (!net.addFriend(a, b)) error(w[2] + " and " + w[3] + " are already friends");
        } else if (cmd == "list friends") {
            if (w.size() != 3) { error("usage: LIST FRIENDS <username>"); continue; }
            int u = user(w[2]);
            if (u != -1) printList(net.listFriends(u));
        } else if (cmd == "suggest friends") {
            int n;
            if (w.size() != 4 || !toInt(w[3], n)) { error("usage: SUGGEST FRIENDS <username> <N>"); continue; }
            if (n < 0) { error("N must be non-negative"); continue; }
            int u = user(w[2]);
            if (u != -1 && n > 0) printList(net.suggestFriends(u, n));   // N = 0 prints nothing
        } else if (cmd == "degrees of") {
            if (w.size() != 5 || w[2] != "separation") { error("usage: DEGREES OF SEPARATION <user1> <user2>"); continue; }
            int a = user(w[3]), b = user(w[4]);
            if (a != -1 && b != -1) std::cout << net.degreesOfSeparation(a, b) << "\n";
        } else if (cmd == "add post") {
            // Content is the text between the first and last double quote.
            size_t open = line.find('"'), close = line.rfind('"');
            if (w.size() < 4 || open == std::string::npos || close == open) {
                error("usage: ADD POST <username> \"<post content>\""); continue;
            }
            int u = user(w[2]);
            if (u != -1) net.addPost(u, lower(line.substr(open + 1, close - open - 1)));
        } else if (cmd == "output posts") {
            int n;
            if (w.size() != 4 || !toInt(w[3], n) || n < -1) { error("usage: OUTPUT POSTS <username> <N> (N >= 0 or -1)"); continue; }
            int u = user(w[2]);
            if (u != -1) for (const std::string& p : net.outputPosts(u, n)) std::cout << p << "\n";
        } else {
            error("unknown command '" + line + "'");
        }
    }
    return 0;
}
