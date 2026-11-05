// csv_loader.h
// Reads the cleaned dataset (format described in data/README.md) into Songs.

#pragma once

#include <string>
#include <vector>

#include "song.h"

// Splits one CSV line into its fields.
//   - Fields are separated by commas.
//   - A field wrapped in double quotes may contain commas.
//   - Inside a quoted field, "" stands for one literal " character.
// Example:  t004,"Rain, Again",B  ->  {"t004", "Rain, Again", "B"}
// Limitation: a quoted field may not contain a line break. Our cleaning script
// never writes one.
std::vector<std::string> splitCsvLine(const std::string& line);

// Loads every song from the CSV file at `path`.
// The header must start with track_id,track_name,artist_name,language; every
// column after that is a feature, so all songs get the same number of features.
// Throws std::runtime_error (with the line number) if the file can't be opened
// or a row is malformed, instead of silently loading bad data.
std::vector<Song> loadSongs(const std::string& path);
