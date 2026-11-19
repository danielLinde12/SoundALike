// kd_tree.cpp
// See kd_tree.h for what each function does and how the tree works.

#include "kd_tree.h"

#include <algorithm>
#include <limits>
#include <numeric>
#include <stdexcept>

// ---------------------------------------------------------------------------
// Building
// ---------------------------------------------------------------------------

KDTree::KDTree(const std::vector<Song>& songs) : songs_(songs) {
    if (songs.empty()) {
        return;  // empty tree: root_ stays null
    }

    // Every song must have the same number of features, or "split on
    // feature 3" would not mean anything for some songs.
    dimensions_ = songs[0].features.size();
    for (const Song& song : songs) {
        if (song.features.size() != dimensions_) {
            throw std::invalid_argument("all songs must have the same number of features");
        }
    }

    // The tree is built from song POSITIONS (0, 1, 2, ..., n-1), not copies
    // of the songs. We reorder this list while building; songs_ never moves.
    std::vector<size_t> indices(songs.size());
    std::iota(indices.begin(), indices.end(), 0);  // fills 0, 1, 2, ...

    root_ = build(indices, 0, indices.size(), 0);
}

// Builds a subtree from the songs whose positions are in indices[begin, end).
std::unique_ptr<KDTree::Node> KDTree::build(std::vector<size_t>& indices, size_t begin,
                                            size_t end, size_t depth) {
    if (begin >= end) {
        return nullptr;  // no songs left: empty subtree
    }

    // Cycle through the features: depth 0 -> feature 0, depth 1 -> feature 1, ...
    size_t axis = depth % dimensions_;
    size_t mid = begin + (end - begin) / 2;

    // Put the median song (on this axis) at position `mid`. std::nth_element
    // only partly sorts: afterwards everything before `mid` is <= it and
    // everything after is >= it, which is exactly the split we need. It runs
    // in O(n) on average, cheaper than a full sort's O(n log n).
    // Ties on the feature are broken by song position so the tree is always
    // built the same way.
    auto byAxis = [&](size_t a, size_t b) {
        double va = songs_[a].features[axis];
        double vb = songs_[b].features[axis];
        if (va != vb) {
            return va < vb;
        }
        return a < b;
    };
    std::nth_element(indices.begin() + begin, indices.begin() + mid,
                     indices.begin() + end, byAxis);

    auto node = std::make_unique<Node>();
    node->index = indices[mid];
    node->axis = axis;
    node->left = build(indices, begin, mid, depth + 1);     // the smaller half
    node->right = build(indices, mid + 1, end, depth + 1);  // the larger half
    return node;
}

// ---------------------------------------------------------------------------
// Size and shape (used by tests)
// ---------------------------------------------------------------------------

size_t KDTree::size() const {
    return root_ ? songs_.size() : 0;
}

size_t KDTree::height() const {
    return heightOf(root_.get());
}

size_t KDTree::heightOf(const Node* node) {
    if (node == nullptr) {
        return 0;
    }
    return 1 + std::max(heightOf(node->left.get()), heightOf(node->right.get()));
}

bool KDTree::isValid() const {
    // Start with no limits on any feature, then narrow them on the way down.
    double inf = std::numeric_limits<double>::infinity();
    std::vector<double> lo(dimensions_, -inf);
    std::vector<double> hi(dimensions_, inf);
    std::vector<bool> seen(songs_.size(), false);

    if (!isValidNode(root_.get(), lo, hi, seen)) {
        return false;
    }
    // Every song must have been visited exactly once.
    return std::all_of(seen.begin(), seen.end(), [](bool s) { return s; });
}

bool KDTree::isValidNode(const Node* node, std::vector<double>& lo,
                         std::vector<double>& hi, std::vector<bool>& seen) const {
    if (node == nullptr) {
        return true;
    }
    if (node->index >= songs_.size() || seen[node->index]) {
        return false;  // a bad position, or a song that appears twice
    }
    seen[node->index] = true;

    // This song must fit inside every limit set by the splits above it.
    const std::vector<double>& point = songs_[node->index].features;
    for (size_t d = 0; d < dimensions_; d++) {
        if (point[d] < lo[d] || point[d] > hi[d]) {
            return false;
        }
    }

    // Going left, nothing may be bigger than this song on its axis;
    // going right, nothing may be smaller. Save the old limit, narrow it,
    // check the child, then put it back for the other side.
    size_t a = node->axis;
    double value = point[a];

    double oldHi = hi[a];
    hi[a] = value;
    bool leftOk = isValidNode(node->left.get(), lo, hi, seen);
    hi[a] = oldHi;

    double oldLo = lo[a];
    lo[a] = value;
    bool rightOk = isValidNode(node->right.get(), lo, hi, seen);
    lo[a] = oldLo;

    return leftOk && rightOk;
}
