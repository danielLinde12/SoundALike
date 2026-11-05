// test_loader.cpp
// Checks splitCsvLine and loadSongs against data/sample_songs.csv, whose
// contents we know by hand.
//
// Build and run from the repo root:
//   g++ -std=c++17 -Wall -Wextra -Isrc tests/test_loader.cpp src/csv_loader.cpp -o test_loader
//   ./test_loader

#include <cmath>
#include <iostream>
#include <string>

#include "csv_loader.h"

static int failures = 0;

// Prints a message for a failed check and keeps going, so one run reports
// every failure instead of stopping at the first one.
static void check(bool condition, const std::string& description) {
    if (!condition) {
        std::cout << "FAIL: " << description << "\n";
        failures++;
    }
}

static bool closeTo(double a, double b) {
    return std::fabs(a - b) < 1e-9;
}

static void testSplitCsvLine() {
    auto plain = splitCsvLine("a,b,c");
    check(plain.size() == 3 && plain[0] == "a" && plain[2] == "c", "plain fields");

    auto comma = splitCsvLine("t004,\"Rain, Again\",B");
    check(comma.size() == 3 && comma[1] == "Rain, Again", "comma inside quotes");

    auto quote = splitCsvLine("\"The \"\"Big\"\" Song\",x");
    check(quote.size() == 2 && quote[0] == "The \"Big\" Song", "escaped quotes");

    auto empty = splitCsvLine("a,,c");
    check(empty.size() == 3 && empty[1].empty(), "empty middle field");
}

static void testLoadSample() {
    std::vector<Song> songs = loadSongs("data/sample_songs.csv");
    check(songs.size() == 6, "sample has 6 songs");
    if (songs.size() != 6) {
        return;  // the checks below would index out of range
    }

    check(songs[0].id == "t001" && songs[0].name == "Morning Run", "first song labels");
    check(songs[0].artist == "Test Artist A" && songs[0].language == "English",
          "first song artist and language");
    check(songs[0].features.size() == 6, "6 features per song");
    check(closeTo(songs[0].features[0], 0.80) && closeTo(songs[0].features[5], 0.85),
          "first song feature values");

    check(songs[3].name == "Rain, Again" && songs[3].language == "Korean",
          "quoted name with comma");
    check(songs[5].name == "The \"Big\" Song", "quoted name with quotes");
}

static void testBadFileThrows() {
    bool threw = false;
    try {
        loadSongs("data/does_not_exist.csv");
    } catch (const std::runtime_error&) {
        threw = true;
    }
    check(threw, "missing file throws");
}

int main() {
    testSplitCsvLine();
    testLoadSample();
    testBadFileThrows();

    if (failures == 0) {
        std::cout << "All loader tests passed.\n";
        return 0;
    }
    std::cout << failures << " check(s) failed.\n";
    return 1;
}
