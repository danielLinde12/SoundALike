// test_brute_force.cpp
// Checks bruteForceKNearest on tiny cases whose answers we can work out by hand.
//
// Build and run from the repo root:
//   g++ -std=c++17 -Wall -Wextra -Isrc tests/test_brute_force.cpp src/brute_force.cpp src/csv_loader.cpp -o test_brute_force
//   ./test_brute_force

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>

#include "brute_force.h"
#include "csv_loader.h"

static int failures = 0;

static void check(bool condition, const std::string& description) {
    if (!condition) {
        std::cout << "FAIL: " << description << "\n";
        failures++;
    }
}

static bool closeTo(double a, double b) {
    return std::fabs(a - b) < 1e-9;
}

// Makes a Song with only features set (labels don't matter for distance).
static Song point(std::vector<double> features) {
    Song s;
    s.features = features;
    return s;
}

static void testSquaredDistance() {
    // (0,0) to (3,4): 3^2 + 4^2 = 25 (the 3-4-5 triangle, before the sqrt)
    check(closeTo(squaredDistance({0, 0}, {3, 4}), 25.0), "3-4-5 distance");
    check(closeTo(squaredDistance({1, 2}, {1, 2}), 0.0), "same point is 0");
}

static void testSmall2D() {
    // Index:            0       1       2       3
    std::vector<Song> songs = {point({0, 0}), point({1, 0}), point({0, 2}), point({3, 3})};
    // Squared distances from (0,0): 0, 1, 4, 18
    auto result = bruteForceKNearest(songs, {0, 0}, 2);
    check(result.size() == 2, "k=2 returns 2");
    check(result.size() == 2 && result[0].index == 0 && result[1].index == 1,
          "two nearest to (0,0) are 0 then 1");
    check(result.size() == 2 && closeTo(result[1].squaredDistance, 1.0),
          "second distance is 1");

    // Query (3,2): squared distances 13, 8, 9, 1 -> order 3, 1, 2, 0
    auto all = bruteForceKNearest(songs, {3, 2}, 4);
    check(all.size() == 4 && all[0].index == 3 && all[1].index == 1 &&
              all[2].index == 2 && all[3].index == 0,
          "full order from (3,2) is 3,1,2,0");
}

static void testTies() {
    // (1,0) and (0,1) are both exactly 1 away from (0,0); smaller index wins.
    std::vector<Song> songs = {point({0, 1}), point({1, 0})};
    auto result = bruteForceKNearest(songs, {0, 0}, 1);
    check(result.size() == 1 && result[0].index == 0, "tie goes to smaller index");
}

static void testEdgeCases() {
    std::vector<Song> songs = {point({0, 0}), point({1, 1})};
    check(bruteForceKNearest(songs, {0, 0}, 0).empty(), "k=0 returns nothing");
    check(bruteForceKNearest(songs, {0, 0}, 5).size() == 2, "k > n returns all songs");
    check(bruteForceKNearest({}, {0, 0}, 3).empty(), "no songs returns nothing");

    bool threw = false;
    try {
        bruteForceKNearest(songs, {0, 0, 0}, 1);  // 3 coordinates vs 2
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    check(threw, "mismatched feature count throws");
}

static void testSampleCsv() {
    // From data/sample_songs.csv: the nearest song to "Morning Run" (index 0)
    // is itself (distance 0), then "Summer Jump" (index 4). Differences are
    // 0.05 in five features and 0.10 in one: 5*0.0025 + 0.01 = 0.0225.
    std::vector<Song> songs = loadSongs("data/sample_songs.csv");
    auto result = bruteForceKNearest(songs, songs[0].features, 2);
    check(result.size() == 2 && result[0].index == 0, "nearest to itself is itself");
    check(result.size() == 2 && result[1].index == 4 &&
              closeTo(result[1].squaredDistance, 0.0225),
          "next nearest to Morning Run is Summer Jump");
}

int main() {
    testSquaredDistance();
    testSmall2D();
    testTies();
    testEdgeCases();
    testSampleCsv();

    if (failures == 0) {
        std::cout << "All brute-force tests passed.\n";
        return 0;
    }
    std::cout << failures << " check(s) failed.\n";
    return 1;
}
