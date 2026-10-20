#pragma once
#include <iostream>
#include <sstream>
#include <string>
#include "File.hpp"
#include "HashMap.hpp"
#include "MaxHeap.hpp"

// Heap orderings used by RECENT_FILES and BIGGEST_TREES.
struct OlderFirstIsLess {
    bool operator()(File* a, File* b) const { return a->last_modified < b->last_modified; }
};
struct SmallerTreeIsLess {
    bool operator()(File* a, File* b) const { return a->total_versions < b->total_versions; }
};

// Parses commands and keeps every file in a filename -> File* map.
class FileSystem {
    HashMap<std::string, File*> files;

    static void error(const std::string& msg) { std::cout << "Error: " << msg << "\n"; }

    static std::string timeString(time_t t) {
        char buf[32];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&t));
        return buf;
    }

    // Look up a file, printing an error if it is missing.
    File* find(const std::string& name) {
        File** f = files.get(name);
        if (!f) { error("file '" + name + "' does not exist"); return nullptr; }
        return *f;
    }

    // Parse an optional count; -1 means "all". Returns false on bad input.
    static bool parseCount(const std::string& s, int& out) {
        if (s.empty()) { out = -1; return true; }
        try { size_t used; out = std::stoi(s, &used); return used == s.size() && out >= 0; }
        catch (...) { return false; }
    }

    template <typename Less>
    void printTop(const std::string& countText, bool showVersions) {
        int k;
        if (!parseCount(countText, k)) { error("count must be a non-negative integer"); return; }
        MaxHeap<File*, Less> heap(files.values());   // O(N) build
        for (int i = 0; !heap.empty() && (k < 0 || i < k); i++) {   // k pops, O(k log N)
            File* f = heap.pop();
            std::cout << f->name << " ";
            if (showVersions) std::cout << f->total_versions << "\n";
            else std::cout << timeString(f->last_modified) << "\n";
        }
    }

public:
    ~FileSystem() { for (File* f : files.values()) delete f; }

    // Handle one input line. Returns false on EXIT.
    bool execute(const std::string& line) {
        std::istringstream in(line);
        std::string cmd, name, rest;
        in >> cmd >> name;
        std::getline(in, rest);
        if (!rest.empty() && rest[0] == ' ') rest.erase(0, 1);   // rest of line after filename

        if (cmd.empty()) return true;
        if (cmd == "EXIT") return false;

        // "RECENT FILES 3" / "BIGGEST TREES 3" -> treat like RECENT_FILES 3.
        if ((cmd == "RECENT" && name == "FILES") || (cmd == "BIGGEST" && name == "TREES")) {
            cmd += "_" + name;
            std::istringstream again(rest);
            name.clear();
            again >> name;
        }
        if (cmd == "RECENT_FILES")  { printTop<OlderFirstIsLess>(name, false); return true; }
        if (cmd == "BIGGEST_TREES") { printTop<SmallerTreeIsLess>(name, true); return true; }

        bool known = cmd == "CREATE" || cmd == "READ" || cmd == "INSERT" || cmd == "UPDATE" ||
                     cmd == "SNAPSHOT" || cmd == "ROLLBACK" || cmd == "HISTORY";
        if (!known) { error("unknown command '" + cmd + "'"); return true; }
        if (name.empty()) { error(cmd + " needs a filename"); return true; }

        if (cmd == "CREATE") {
            if (files.contains(name)) error("file '" + name + "' already exists");
            else { files.put(name, new File(name)); std::cout << "Created " << name << "\n"; }
            return true;
        }

        File* f = find(name);
        if (!f) return true;

        if (cmd == "READ") {
            std::cout << f->read() << "\n";
        } else if (cmd == "INSERT" || cmd == "UPDATE") {
            if (rest.empty()) { error(cmd + " needs content"); return true; }
            f->write(rest, cmd == "INSERT");
            std::cout << cmd << " ok (active version " << f->active_version->version_id << ")\n";
        } else if (cmd == "SNAPSHOT") {
            if (rest.empty()) error("SNAPSHOT needs a message");
            else if (!f->snapshot(rest)) error("version " + std::to_string(f->active_version->version_id) + " is already a snapshot");
            else std::cout << "Snapshot of version " << f->active_version->version_id << " saved\n";
        } else if (cmd == "ROLLBACK") {
            if (rest.empty()) {
                if (!f->rollbackToParent()) { error("already at root version"); return true; }
            } else {
                int id;
                if (!parseCount(rest, id)) { error("version id must be a non-negative integer"); return true; }
                if (!f->rollbackTo(id)) { error("version " + rest + " does not exist"); return true; }
            }
            std::cout << "Active version is now " << f->active_version->version_id << "\n";
        } else if (cmd == "HISTORY") {
            for (TreeNode* n : f->history())
                std::cout << n->version_id << " | " << timeString(n->snapshot_timestamp) << " | " << n->message << "\n";
        }
        return true;
    }
};
