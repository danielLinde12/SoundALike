// test_kd_tree.cpp
// Checks that the k-d tree is built correctly (right size, balanced height,
// split rule holds at every node) and that its nearest-song search returns
// exactly what brute force returns.
//
// Build and run from the repo root:
//   g++ -std=c++17 -Wall -Wextra -Isrc tests/test_kd_tree.cpp src/kd_tree.cpp src/brute_force.cpp -o test_kd_tree
//   ./test_kd_tree

#include <iostream>
#include <random>
#include <stdexcept>
#include <string>

#include "brute_force.h"
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

// ---------------------------------------------------------------------------
// Search tests (k = 1)
// ---------------------------------------------------------------------------

static void testNearestSmall() {
    /* Same 7 songs as testSmall2D. The tree is:
                       (4,1)            split on x
                     /       \
                (3,6)         (7,5)     split on y
               /     \       /     \
           (2,3)   (1,9) (6,2)   (5,8)
    */
    std::vector<Song> songs = {point({1, 9}), point({2, 3}), point({3, 6}), point({4, 1}),
                               point({5, 8}), point({6, 2}), point({7, 5})};
    KDTree tree(songs);

    // Query (6.5, 3). Closest is (6,2): 0.5^2 + 1^2 = 1.25.
    // The search visits (4,1) -> (7,5) -> (6,2), then skips (5,8)
    // (its split line y = 5 is 2 away, and 2^2 = 4 > 1.25) and skips the
    // whole left subtree (split line x = 4 is 2.5 away, 6.25 > 1.25).
    size_t count = 0;
    auto result = tree.kNearest({6.5, 3}, 1, &count);
    check(result.size() == 1 && result[0].index == 5, "nearest to (6.5,3) is (6,2)");
    check(count == 3, "search for (6.5,3) computes only 3 of 7 distances");

    // Query (1, 8.5): closest is (1,9), on the far left of the tree.
    result = tree.kNearest({1, 8.5}, 1);
    check(result.size() == 1 && result[0].index == 0, "nearest to (1,8.5) is (1,9)");

    // A query exactly on a song finds that song at distance 0.
    result = tree.kNearest({4, 1}, 1);
    check(result.size() == 1 && result[0].index == 3 && result[0].squaredDistance == 0,
          "query on (4,1) finds itself");
}

static void testNearestTies() {
    // 20 identical songs: all are the same distance from any query, so the
    // tie rule says the smallest index (0) wins, just like brute force.
    std::vector<Song> songs(20, point({0.3, 0.3, 0.3}));
    KDTree tree(songs);
    auto result = tree.kNearest({0.9, 0.1, 0.5}, 1);
    check(result.size() == 1 && result[0].index == 0, "ties: smallest index wins");
}

static void testTieAcrossSplit() {
    // A tie that sits exactly on a split line, on the side searched second.
    // Songs: 0 = (2,0), 1 = (3,0), 2 = (2,5). The root is (2,5), split on
    // x = 2; song 0 (x = 2) goes left and song 1 (x = 3) goes right.
    //
    // Query (2.5, 0): songs 0 and 1 are both 0.25 away (a tie), so song 0
    // must win. The search finds song 1 first (right side), then must still
    // check the left side, because the split line is also exactly 0.25 away.
    // This only works because the skip test uses <= instead of <.
    std::vector<Song> songs = {point({2, 0}), point({3, 0}), point({2, 5})};
    KDTree tree(songs);
    auto result = tree.kNearest({2.5, 0}, 1);
    check(result.size() == 1 && result[0].index == 0,
          "tie on a split line: smaller index wins (needs <= in the skip test)");
}

static void testNearestMatchesBruteForce() {
    // Many random queries: the tree must give exactly brute force's answer.
    for (size_t dims : {2, 6, 10}) {
        std::vector<Song> songs = randomSongs(5000, dims, 7);
        KDTree tree(songs);
        std::vector<Song> queries = randomSongs(1000, dims, 99);

        size_t mismatches = 0;
        size_t totalCount = 0;
        for (const Song& q : queries) {
            size_t count = 0;
            auto fromTree = tree.kNearest(q.features, 1, &count);
            auto fromBrute = bruteForceKNearest(songs, q.features, 1);
            totalCount += count;
            if (fromTree.size() != 1 || fromTree[0].index != fromBrute[0].index) {
                mismatches++;
            }
        }
        std::string label = std::to_string(dims) + "D";
        check(mismatches == 0, label + ": all 1000 nearest songs match brute force");

        // In 2D the tree should skip almost everything (brute force checks 5000).
        if (dims == 2) {
            check(totalCount / queries.size() < 100,
                  "2D: average search computes fewer than 100 of 5000 distances");
        }
        std::cout << "  " << label << ": average distances per search = "
                  << totalCount / queries.size() << " of " << songs.size() << "\n";
    }
}

static void testSearchEdgeCases() {
    std::vector<Song> none;
    KDTree empty(none);
    check(empty.kNearest({0.5, 0.5}, 1).empty(), "empty tree returns nothing");

    std::vector<Song> songs = {point({0.1, 0.2}), point({0.3, 0.4})};
    KDTree tree(songs);
    check(tree.kNearest({0.1, 0.2}, 0).empty(), "k = 0 returns nothing");

    bool threw = false;
    try {
        tree.kNearest({0.1}, 1);
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    check(threw, "query with the wrong number of features throws");
}

int main() {
    testEmpty();
    testOneSong();
    testSmall2D();
    testDuplicates();
    testRandomBalanced();
    testMismatchedFeatures();
    testNearestSmall();
    testNearestTies();
    testTieAcrossSplit();
    testNearestMatchesBruteForce();
    testSearchEdgeCases();

    if (failures == 0) {
        std::cout << "All k-d tree tests passed.\n";
        return 0;
    }
    std::cout << failures << " test(s) failed.\n";
    return 1;
}
