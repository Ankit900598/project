#pragma once
#include <algorithm>
#include <string>
#include <vector>

// One post. Key = creation time (a unique, increasing counter).
struct PostNode {
    long long time;
    std::string content;
    PostNode* left = nullptr;
    PostNode* right = nullptr;
    int height = 1;
    PostNode(long long t, const std::string& c) : time(t), content(c) {}
};

// Self-balancing BST of one user's posts, ordered by time.
class AVLTree {
    PostNode* root = nullptr;

    static int height(PostNode* n) { return n ? n->height : 0; }
    static int balance(PostNode* n) { return n ? height(n->left) - height(n->right) : 0; }
    static void update(PostNode* n) { n->height = 1 + std::max(height(n->left), height(n->right)); }

    // Right rotation: left child x becomes the new subtree root, y becomes x's right child.
    static PostNode* rotateRight(PostNode* y) {
        PostNode* x = y->left;
        y->left = x->right;
        x->right = y;
        update(y);
        update(x);
        return x;
    }

    static PostNode* rotateLeft(PostNode* x) {
        PostNode* y = x->right;
        x->right = y->left;
        y->left = x;
        update(x);
        update(y);
        return y;
    }

    static PostNode* insert(PostNode* node, long long time, const std::string& content) {
        if (!node) return new PostNode(time, content);
        if (time < node->time) node->left = insert(node->left, time, content);
        else node->right = insert(node->right, time, content);   // keys are unique

        update(node);
        int b = balance(node);
        if (b > 1 && balance(node->left) < 0) node->left = rotateLeft(node->left);     // Left-Right
        if (b > 1) return rotateRight(node);                                           // Left-Left
        if (b < -1 && balance(node->right) > 0) node->right = rotateRight(node->right); // Right-Left
        if (b < -1) return rotateLeft(node);                                           // Right-Right
        return node;
    }

    // Right subtree first = newest first. Stops after `limit` posts (-1 = all).
    static void newestFirst(PostNode* node, int limit, std::vector<std::string>& out) {
        if (!node || (limit >= 0 && (int)out.size() >= limit)) return;
        newestFirst(node->right, limit, out);
        if (limit < 0 || (int)out.size() < limit) out.push_back(node->content);
        newestFirst(node->left, limit, out);
    }

    static void destroy(PostNode* node) {
        if (!node) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

public:
    AVLTree() = default;
    ~AVLTree() { destroy(root); }
    AVLTree(const AVLTree&) = delete;
    AVLTree& operator=(const AVLTree&) = delete;

    void add(long long time, const std::string& content) { root = insert(root, time, content); }

    std::vector<std::string> recent(int limit) const {
        std::vector<std::string> out;
        newestFirst(root, limit, out);
        return out;
    }
};
