# SoundAlike — project context for Claude Code

SoundAlike is our CSC 212 (Data Structures and Abstractions, URI, Fall 2026) final project.
You pick a song, and the program finds the songs that sound most similar to it, optionally in a
different language (e.g., "Korean songs that sound like this English song").

Team: Daniel Linde and Kevin Chen (2 people).
Repo: github.com/danielLinde12/SoundALike

## The core idea

- Each song is described by numeric audio features (e.g., energy, danceability, valence,
  acousticness, tempo, loudness), each scaled to 0–1.
- Each song is a point in d-dimensional space (~6 features). Similar songs = closest points
  (Euclidean distance).
- We implement a **k-d tree** ourselves in C++ to find the k nearest songs quickly.
- **Baseline:** brute force — compute the distance to every song and keep the k closest.
- **Problem framing (approved by our TA):** we are not competing with Spotify's AI/server-farm
  recommendations, which return approximate results. Our goal is finding the *exact* most similar
  songs quickly on one ordinary laptop. The k-d tree must always return exactly the same songs as
  brute force.

## Language feature

- Dataset: Kaggle "Spotify Tracks Dataset (Updated Every Week)" by gauthamvijayaraj, ~60k tracks
  with audio features and a `language` column (English, Hindi, Tamil, Telugu, Malayalam, Korean).
- Language is a label, not a feature, so it does NOT go into the k-d tree's coordinates.
- Design: build **one k-d tree per language**. To find Korean songs similar to an English song,
  search the Korean tree using the English song's feature vector.

## Hard requirements from the assignment (do not violate)

- The core data structure (k-d tree) and the brute-force baseline must be **written by us in C++**.
  Do not use a library k-d tree, nearest-neighbor library, or similar in the core.
- Other languages/libraries are fine for everything around the core: data cleaning (Python),
  plotting, and the interface (likely Python, e.g. Streamlit, calling our C++ program).
- **Tests** must check the structure's correctness on its own (hand-checkable small cases, plus
  randomized tests comparing k-d tree results to brute force).
- **Benchmarks** with plots of time vs. input size (like Lab 1):
  - Experiment 1: search time and number of distance calculations vs. number of songs.
  - Experiment 2: fixed number of songs, number of features varied from 2 to 10, to show where
    the k-d tree stops helping.
- We must be able to **explain every line** at the showcase. Prefer clear, readable,
  well-commented code over clever code. When writing code, briefly explain how it works and why.
- The commit history is graded per person: make small, frequent commits with clear messages.
- The final report (REPORT.md) needs an **AI usage** section: where AI was used, what it got
  wrong, and what we fixed or rejected. Log notable AI help in `AI_USAGE.md` as we go.

## Repository layout

```
src/          C++ source (Song struct, CSV loader, brute force, k-d tree)
tests/        Correctness tests
benchmarks/   Timing experiments (C++) and plotting scripts (Python)
data/         Cleaning script and cleaned dataset
```

Use C++17. Keep the build simple (a Makefile or a short g++ command, documented in README.md).

## Roles

- **Daniel:** k-d tree (build + k-nearest-neighbor search) and its tests; this week the C++
  Song struct, CSV loader, and brute-force search.
- **Kevin:** data cleaning (Python), brute-force baseline support, and the interface.
- **Both:** benchmarks, plots, report, README.

## Schedule

| Week of | Goal |
| --- | --- |
| Nov 2 | Clean dataset; get brute-force search working |
| Nov 9 and Nov 16 | Build the k-d tree and its search; write tests |
| Nov 23 | Finish and debug the tree; start the interface |
| Nov 30 | Run experiments and make plots |
| Dec 7 | Finish the interface; draft the report |
| Dec 14 | Final fixes. Repo + REPORT.md due Dec 17 11:59pm; showcase Dec 18 |

## This week's tasks (week of Nov 2)

Data (Kevin):
1. Explore the dataset: missing values, duplicates, songs per language.
2. Python script: keep name, artist, language, and ~6 features; drop bad rows; scale features to 0–1.
3. Save a cleaned CSV in `data/`.

C++ (Daniel):
1. `Song` struct: name, artist, language, feature array.
2. Load the cleaned CSV into `std::vector<Song>` (start with a small hand-made CSV).
3. Brute-force k-nearest search, with an optional language filter later.
4. A small test on hand-checkable data.

Agree on the CSV format (column names and order) first so the data and loader match.

## Stretch goals (only after the core works)

Genre filter, 2D song map (PCA computed in Python, for display only), and a ball tree to compare
against the k-d tree with many features.
