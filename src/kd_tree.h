// kd_tree.h
// A k-d tree: finds the k songs closest to a query point without checking
// every song. It must always return exactly what bruteForceKNearest returns
// (same songs, same order, same tie-breaking rule).

#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "brute_force.h"  // Neighbor, isCloser, squaredDistance
#include "song.h"

// How it works, in short:
//
// Each node holds one song and splits the remaining songs on one feature
// (its "axis"). Songs with a smaller or equal value on that feature go left,
// larger or equal go right (songs tied with the split can land on either side). The axis cycles with depth: depth 0 splits on feature 0
// (danceability), depth 1 on feature 1 (energy), and so on, wrapping around.
//
// Building: at each node we pick the MEDIAN song on that node's axis, so half
// the songs go each way and the tree stays balanced (height about log2 n).
//
// Searching: we walk down toward the query like a BST search, then on the way
// back up we only visit the other side of a split if it could still hold a
// song closer than the k-th best found so far. Skipping those branches is
// where the speedup over brute force comes from.
class KDTree {
public:
    // Builds the tree over `songs`. Every song must have the same number of
    // features (throws std::invalid_argument if not).
    //
    // The tree stores positions in `songs`, not copies of the songs, so
    // `songs` must stay alive and unchanged while the tree is used.
    explicit KDTree(const std::vector<Song>& songs);

    // Returns the k songs closest to `query`, closest first, with ties broken
    // by smaller index (the same rule as brute force). If the tree has fewer
    // than k songs, returns all of them.
    //
    // If `distanceCount` is not null, it is set to how many distances this
    // search computed. Benchmarks use this to compare against brute force,
    // which always computes n distances.
    //
    // Throws std::invalid_argument if the query has the wrong number of features.
    std::vector<Neighbor> kNearest(const std::vector<double>& query, size_t k,
                                   size_t* distanceCount = nullptr) const;

    size_t size() const;    // number of songs in the tree
    size_t height() const;  // longest root-to-leaf path in nodes (0 if empty); used in tests

    // For tests: checks the k-d tree rule at every node (left side <= split,
    // right side >= split, on that node's axis) and that every song appears
    // exactly once.
    bool isValid() const;

private:
    struct Node {
        size_t index;  // position of this node's song in songs_
        size_t axis;   // which feature this node splits on
        // unique_ptr deletes the child automatically when the node is
        // deleted, so the whole tree is freed with no manual delete calls.
        std::unique_ptr<Node> left;   // songs with a value <= this song's, on `axis`
        std::unique_ptr<Node> right;  // songs with a value >= this song's, on `axis`
    };

    const std::vector<Song>& songs_;  // the songs the tree was built from
    size_t dimensions_ = 0;           // number of features per song
    std::unique_ptr<Node> root_;

    // Builds a subtree from indices[begin, end) at the given depth.
    std::unique_ptr<Node> build(std::vector<size_t>& indices, size_t begin,
                                size_t end, size_t depth);

    // Recursive search used by kNearest (written in step 3).
    void search(const Node* node, const std::vector<double>& query, size_t k,
                std::vector<Neighbor>& best, size_t& distanceCount) const;

    static size_t heightOf(const Node* node);

    // Helper for isValid. lo/hi are the allowed range for each feature at
    // this node, narrowed by every split on the path from the root.
    bool isValidNode(const Node* node, std::vector<double>& lo,
                     std::vector<double>& hi, std::vector<bool>& seen) const;
};
