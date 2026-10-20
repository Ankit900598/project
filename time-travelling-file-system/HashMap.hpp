#pragma once
#include <string>
#include <vector>

// Hash functions: integers hash to themselves, strings use a polynomial hash.
inline size_t hashKey(int key) { return static_cast<size_t>(key); }
inline size_t hashKey(const std::string& key) {
    size_t h = 0;
    for (char c : key) h = h * 31 + static_cast<unsigned char>(c);
    return h;
}

// Hash map with separate chaining. Each bucket is a vector of (key, value) pairs.
template <typename K, typename V>
class HashMap {
    struct Entry { K key; V value; };
    std::vector<std::vector<Entry>> buckets;
    size_t count = 0;

    size_t indexOf(const K& key) const { return hashKey(key) % buckets.size(); }

    // Double the number of buckets and re-insert every entry.
    void grow() {
        std::vector<std::vector<Entry>> old = std::move(buckets);
        buckets.assign(old.size() * 2, {});
        for (auto& bucket : old)
            for (auto& e : bucket) buckets[indexOf(e.key)].push_back(e);
    }

public:
    HashMap() : buckets(16) {}

    // Insert or overwrite.
    void put(const K& key, const V& value) {
        for (auto& e : buckets[indexOf(key)])
            if (e.key == key) { e.value = value; return; }
        buckets[indexOf(key)].push_back({key, value});
        count++;
        if (count > buckets.size() * 3 / 4) grow();   // load factor 0.75
    }

    // Returns pointer to the value, or nullptr if the key is missing.
    V* get(const K& key) {
        for (auto& e : buckets[indexOf(key)])
            if (e.key == key) return &e.value;
        return nullptr;
    }

    bool contains(const K& key) { return get(key) != nullptr; }
    size_t size() const { return count; }

    // All values (used to build heaps over every file).
    std::vector<V> values() const {
        std::vector<V> out;
        for (auto& bucket : buckets)
            for (auto& e : bucket) out.push_back(e.value);
        return out;
    }
};
