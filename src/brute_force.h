// brute_force.h
// The baseline: find the k nearest songs by checking every song.
// The k-d tree must always return exactly the same results as this.

#pragma once

#include <cstddef>
#include <vector>

#include "song.h"

// One search result: which song (its position in the songs vector) and how
// far it is from the query.
struct Neighbor {
    size_t index;            // position in the songs vector
    double squaredDistance;  // distance squared (see squaredDistance below)
};

// Returns true if neighbor a is "closer" than neighbor b.
// Ties in distance are broken by the smaller index. This makes the answer
// unique, so the k-d tree and brute force can be compared song by song even
// when two songs are exactly the same distance away. The k-d tree must use
// this same rule.
bool isCloser(const Neighbor& a, const Neighbor& b);

// Squared Euclidean distance between two points with the same number of
// coordinates: (a0-b0)^2 + (a1-b1)^2 + ...
// We skip the square root: if x < y then sqrt(x) < sqrt(y), so squared
// distances put songs in the same order and save a sqrt per comparison.
double squaredDistance(const std::vector<double>& a, const std::vector<double>& b);

// Returns the k songs closest to `query`, closest first (ties: smaller index
// first). If there are fewer than k songs, returns all of them.
// Throws std::invalid_argument if a song has a different number of features
// than the query.
std::vector<Neighbor> bruteForceKNearest(const std::vector<Song>& songs,
                                         const std::vector<double>& query,
                                         size_t k);
