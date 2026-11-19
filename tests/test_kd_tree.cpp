// test_kd_tree.cpp
// Checks that the k-d tree is built correctly: right size, balanced height,
// and the split rule holds at every node.
//
// Build and run from the repo root:
//   g++ -std=c++17 -Wall -Wextra -Isrc tests/test_kd_tree.cpp src/kd_tree.cpp src/brute_force.cpp -o test_kd_tree
//   ./test_kd_tree

#include <iostream>
#include <random>
#include <stdexcept>
#include <string>

#include "kd_tree.h"

static int failures = 0;

static void check(bool condition, const std::string& description) {
    if (!condition) {
        std::cout << "FAIL: " << description << "\n";
        failures++;
    }
}

// Makes a Song with only features set (labels don't matter for the tree).
static Song point(std::vector<double> features) {
    Song s;
    s.features = features;
    return s;
}

// n random songs with `dims` features each, values in 0-1.
// A fixed seed means the "random" songs are the same every run.
static std::vector<Song> randomSongs(size_t n, size_t dims, unsigned seed) {
    std::mt19937 rng(seed);
    std::uniform_real_distribution<double> value(0.0, 1.0);
    std::vector<Song> songs(n);
    for (Song& s : songs) {
        for (size_t d = 0; d < dims; d++) {
            s.features.push_back(value(rng));
        }
    }
    return songs;
}

// Smallest height a binary tree with n nodes can have: ceil(log2(n + 1)).
static size_t minHeight(size_t n) {
    size_t h = 0;
    while (((size_t)1 << h) < n + 1) {
        h++;
    }
    return h;
}

static void testEmpty() {
    std::vector<Song> songs;
    KDTree tree(songs);
    check(tree.size() == 0, "empty tree has size 0");
    check(tree.height() == 0, "empty tree has height 0");
    check(tree.isValid(), "empty tree is valid");
}

static void testOneSong() {
    std::vector<Song> songs = {point({0.5, 0.5})};
    KDTree tree(songs);
    check(tree.size() == 1, "one song: size 1");
    check(tree.height() == 1, "one song: height 1");
    check(tree.isValid(), "one song: valid");
}

static void testSmall2D() {
    // 7 songs in 2D. Root splits on x: the median x is 4 (song 3), so
    // songs with x < 4 go left and x > 4 go right. With 7 songs a
    // perfectly balanced tree has height 3.
    std::vector<Song> songs = {point({1, 9}), point({2, 3}), point({3, 6}), point({4, 1}),
                               point({5, 8}), point({6, 2}), point({7, 5})};
    KDTree tree(songs);
    check(tree.size() == 7, "small 2D: size 7");
    check(tree.height() == 3, "small 2D: height 3 (perfectly balanced)");
    check(tree.isValid(), "small 2D: split rule holds");
}

static void testDuplicates() {
    // Many songs with identical features: ties must not break the tree.
    std::vector<Song> songs(20, point({0.3, 0.3, 0.3}));
    KDTree tree(songs);
    check(tree.size() == 20, "duplicates: size 20");
    check(tree.height() == minHeight(20), "duplicates: still balanced");
    check(tree.isValid(), "duplicates: valid");
}

static void testRandomBalanced() {
    // Bigger random trees, in 2, 6, and 10 dimensions (the range of Experiment 2).
    for (size_t dims : {2, 6, 10}) {
        for (size_t n : {10, 100, 1000, 10000}) {
            std::vector<Song> songs = randomSongs(n, dims, 42);
            KDTree tree(songs);
            std::string label = std::to_string(n) + " songs, " + std::to_string(dims) + "D";
            check(tree.size() == n, label + ": size");
            check(tree.height() == minHeight(n), label + ": balanced height");
            check(tree.isValid(), label + ": split rule holds");
        }
    }
}

static void testMismatchedFeatures() {
    std::vector<Song> songs = {point({0.1, 0.2}), point({0.3})};
    bool threw = false;
    try {
        KDTree tree(songs);
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    check(threw, "songs with different feature counts throw");
}

int main() {
    testEmpty();
    testOneSong();
    testSmall2D();
    testDuplicates();
    testRandomBalanced();
    testMismatchedFeatures();

    if (failures == 0) {
        std::cout << "All k-d tree build tests passed.\n";
        return 0;
    }
    std::cout << failures << " test(s) failed.\n";
    return 1;
}
