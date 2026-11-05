// song.h
// The Song struct: one row of the cleaned dataset (see data/README.md).

#pragma once

#include <string>
#include <vector>

// One song from the dataset.
//
// The text fields (id, name, artist, language) are labels for showing results
// and for picking which language's tree to search. They are NOT used to measure
// similarity.
//
// Only `features` is used for distance: each song is a point in d-dimensional
// space, where d = features.size(). Every value is already scaled to 0-1.
struct Song {
    std::string id;        // Spotify track ID (unique)
    std::string name;      // song title
    std::string artist;    // artist name
    std::string language;  // e.g. "English", "Korean"

    // Audio features, in the same order as the CSV columns
    // (danceability, energy, valence, acousticness, tempo, loudness, ...).
    // We use a vector instead of a fixed-size array so the number of features
    // can change (Experiment 2 tries 2 to 10) without changing this struct.
    std::vector<double> features;
};
