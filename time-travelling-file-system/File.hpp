#pragma once
#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include "HashMap.hpp"
#include "TreeNode.hpp"

// A file = a tree of versions plus a pointer to the version we are working on.
class File {
public:
    std::string name;
    TreeNode* root;
    TreeNode* active_version;
    HashMap<int, TreeNode*> version_map;   // version id -> node, O(1) lookup
    int total_versions = 1;
    time_t last_modified;

    explicit File(const std::string& filename) : name(filename) {
        root = new TreeNode(0, "", nullptr);
        root->is_snapshot = true;
        root->message = "Initial snapshot";
        root->snapshot_timestamp = root->created_timestamp;
        active_version = root;
        version_map.put(0, root);
        last_modified = root->created_timestamp;
    }

    ~File() { delete root; }                 // recursively frees all versions
    File(const File&) = delete;
    File& operator=(const File&) = delete;

    std::string read() const { return active_version->content; }

    // INSERT (append=true) or UPDATE (append=false).
    void write(const std::string& text, bool append) {
        std::string newContent = append ? active_version->content + text : text;
        if (active_version->is_snapshot) {
            // Snapshots are frozen: branch off a new child version.
            TreeNode* child = new TreeNode(total_versions, newContent, active_version);
            active_version->children.push_back(child);
            version_map.put(total_versions, child);
            total_versions++;
            active_version = child;
        } else {
            active_version->content = newContent;  // edit in place
        }
        last_modified = time(nullptr);
    }

    // Returns false if the active version is already a snapshot.
    bool snapshot(const std::string& msg) {
        if (active_version->is_snapshot) return false;
        active_version->is_snapshot = true;
        active_version->message = msg;
        active_version->snapshot_timestamp = time(nullptr);
        last_modified = active_version->snapshot_timestamp;
        return true;
    }

    // Jump to a version id. Returns false if it does not exist.
    bool rollbackTo(int id) {
        TreeNode** node = version_map.get(id);
        if (!node) return false;
        active_version = *node;
        return true;
    }

    // Move to parent. Returns false at the root.
    bool rollbackToParent() {
        if (!active_version->parent) return false;
        active_version = active_version->parent;
        return true;
    }

    // Snapshots on the path root -> active (oldest first).
    std::vector<TreeNode*> history() const {
        std::vector<TreeNode*> path;
        for (TreeNode* n = active_version; n; n = n->parent)
            if (n->is_snapshot) path.push_back(n);
        return std::vector<TreeNode*>(path.rbegin(), path.rend());
    }
};
