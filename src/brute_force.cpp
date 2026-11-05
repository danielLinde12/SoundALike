// brute_force.cpp
// See brute_force.h for what each function does.

#include "brute_force.h"

#include <algorithm>
#include <queue>
#include <stdexcept>

bool isCloser(const Neighbor& a, const Neighbor& b) {
    if (a.squaredDistance != b.squaredDistance) {
        return a.squaredDistance < b.squaredDistance;
    }
    return a.index < b.index;  // tie: smaller index counts as closer
}

double squaredDistance(const std::vector<double>& a, const std::vector<double>& b) {
    double sum = 0.0;
    for (size_t i = 0; i < a.size(); i++) {
        double diff = a[i] - b[i];
        sum += diff * diff;
    }
    return sum;
}

// Comparator for the priority queue below. std::priority_queue keeps the
// "largest" element on top, so using isCloser as "less than" puts the
// FARTHEST of the kept neighbors on top, where we can check and remove it.
struct FartherOnTop {
    bool operator()(const Neighbor& a, const Neighbor& b) const {
        return isCloser(a, b);
    }
};

std::vector<Neighbor> bruteForceKNearest(const std::vector<Song>& songs,
                                         const std::vector<double>& query,
                                         size_t k) {
    std::vector<Neighbor> result;
    if (k == 0) {
        return result;
    }

    // Keep the best k seen so far in a max-heap (farthest on top).
    // For each new song: if we have fewer than k, keep it; otherwise it
    // replaces the farthest one only if it is closer. This takes O(n log k)
    // time instead of sorting all n songs (O(n log n)).
    std::priority_queue<Neighbor, std::vector<Neighbor>, FartherOnTop> best;

    for (size_t i = 0; i < songs.size(); i++) {
        if (songs[i].features.size() != query.size()) {
            throw std::invalid_argument("song has a different number of features than the query");
        }
        Neighbor candidate{i, squaredDistance(songs[i].features, query)};

        if (best.size() < k) {
            best.push(candidate);
        } else if (isCloser(candidate, best.top())) {
            best.pop();             // drop the farthest kept neighbor
            best.push(candidate);
        }
    }

    // Empty the heap (it comes out farthest first), then reverse so the
    // result is closest first.
    while (!best.empty()) {
        result.push_back(best.top());
        best.pop();
    }
    std::reverse(result.begin(), result.end());
    return result;
}
