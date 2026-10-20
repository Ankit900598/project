#pragma once
#include <ctime>
#include <string>
#include <vector>

// One version of a file. Versions form a tree (branching after rollbacks).
struct TreeNode {
    int version_id;
    std::string content;
    std::string message;          // snapshot message ("" if not a snapshot)
    bool is_snapshot = false;     // snapshots are immutable
    time_t created_timestamp;
    time_t snapshot_timestamp = 0;
    TreeNode* parent;
    std::vector<TreeNode*> children;

    TreeNode(int id, const std::string& text, TreeNode* par)
        : version_id(id), content(text), created_timestamp(time(nullptr)), parent(par) {}

    // Deleting a node deletes its whole subtree.
    ~TreeNode() {
        for (TreeNode* child : children) delete child;
    }
};
