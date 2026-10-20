#pragma once
#include <utility>
#include <vector>

// Array-based binary max-heap. Less(a, b) returns true if a has lower priority than b.
template <typename T, typename Less>
class MaxHeap {
    std::vector<T> data;
    Less less;

    void siftUp(size_t i) {
        while (i > 0) {
            size_t p = (i - 1) / 2;
            if (!less(data[p], data[i])) break;
            std::swap(data[p], data[i]);
            i = p;
        }
    }

    void siftDown(size_t i) {
        size_t n = data.size();
        while (true) {
            size_t l = 2 * i + 1, r = l + 1, best = i;
            if (l < n && less(data[best], data[l])) best = l;
            if (r < n && less(data[best], data[r])) best = r;
            if (best == i) break;
            std::swap(data[i], data[best]);
            i = best;
        }
    }

public:
    // Build a heap from a list in O(N) (bottom-up heapify).
    explicit MaxHeap(const std::vector<T>& items, Less cmp = Less()) : data(items), less(cmp) {
        for (size_t i = data.size() / 2; i-- > 0;) siftDown(i);
    }

    void push(const T& x) { data.push_back(x); siftUp(data.size() - 1); }
    bool empty() const { return data.empty(); }
    size_t size() const { return data.size(); }
    const T& top() const { return data[0]; }

    T pop() {
        T best = data[0];
        data[0] = data.back();
        data.pop_back();
        if (!data.empty()) siftDown(0);
        return best;
    }
};
