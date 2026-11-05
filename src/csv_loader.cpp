// csv_loader.cpp
// See csv_loader.h for what each function does.

#include "csv_loader.h"

#include <fstream>
#include <stdexcept>

// The first four columns are always text labels, in this order.
static const std::vector<std::string> LABEL_COLUMNS = {
    "track_id", "track_name", "artist_name", "language"};

// Builds an error message that says which line of the file was bad.
static std::runtime_error lineError(int lineNumber, const std::string& message) {
    return std::runtime_error("CSV line " + std::to_string(lineNumber) + ": " + message);
}

std::vector<std::string> splitCsvLine(const std::string& line) {
    std::vector<std::string> fields;
    std::string current;     // the field we are building
    bool inQuotes = false;   // are we between an opening and closing quote?

    for (size_t i = 0; i < line.size(); i++) {
        char c = line[i];

        if (inQuotes) {
            if (c == '"') {
                // Either "" (an escaped quote) or the closing quote.
                if (i + 1 < line.size() && line[i + 1] == '"') {
                    current += '"';
                    i++;  // skip the second quote of the pair
                } else {
                    inQuotes = false;
                }
            } else {
                current += c;  // commas inside quotes are just text
            }
        } else {
            if (c == '"') {
                inQuotes = true;
            } else if (c == ',') {
                fields.push_back(current);  // end of this field
                current.clear();
            } else {
                current += c;
            }
        }
    }
    fields.push_back(current);  // the last field has no comma after it
    return fields;
}

// Removes a trailing '\r' (files saved on Windows end lines with "\r\n").
static void stripCarriageReturn(std::string& line) {
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }
}

// Converts text to a double, requiring the whole field to be a number.
static double parseFeature(const std::string& text, int lineNumber) {
    size_t used = 0;
    double value;
    try {
        value = std::stod(text, &used);
    } catch (const std::exception&) {
        throw lineError(lineNumber, "'" + text + "' is not a number");
    }
    if (used != text.size()) {  // e.g. "0.5abc": stod reads 0.5 and stops
        throw lineError(lineNumber, "'" + text + "' is not a number");
    }
    // The cleaning script scales every feature to 0-1, so anything else means
    // the data was not cleaned correctly.
    // Written as "not (inside the range)" rather than "value < 0 || value > 1"
    // because stod accepts "nan", and every comparison with NaN is false, so
    // the second form would let NaN through.
    if (!(value >= 0.0 && value <= 1.0)) {
        throw lineError(lineNumber, "feature value " + text + " is outside 0-1");
    }
    return value;
}

std::vector<Song> loadSongs(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error("could not open " + path);
    }

    // --- Header row ---
    std::string line;
    if (!std::getline(file, line)) {
        throw std::runtime_error(path + " is empty");
    }
    stripCarriageReturn(line);
    // Some editors put a UTF-8 "byte order mark" (3 invisible bytes) at the
    // start of the file. Remove it so the first column name matches.
    if (line.rfind("\xEF\xBB\xBF", 0) == 0) {
        line.erase(0, 3);
    }

    std::vector<std::string> header = splitCsvLine(line);
    if (header.size() <= LABEL_COLUMNS.size()) {
        throw lineError(1, "header has no feature columns");
    }
    for (size_t i = 0; i < LABEL_COLUMNS.size(); i++) {
        if (header[i] != LABEL_COLUMNS[i]) {
            throw lineError(1, "expected column '" + LABEL_COLUMNS[i] +
                                   "' but found '" + header[i] + "'");
        }
    }
    const size_t numColumns = header.size();
    const size_t numFeatures = numColumns - LABEL_COLUMNS.size();

    // --- Data rows ---
    std::vector<Song> songs;
    int lineNumber = 1;  // the header was line 1
    while (std::getline(file, line)) {
        lineNumber++;
        stripCarriageReturn(line);
        if (line.empty()) {
            continue;  // allow blank lines, e.g. at the end of the file
        }

        std::vector<std::string> fields = splitCsvLine(line);
        if (fields.size() != numColumns) {
            throw lineError(lineNumber, "expected " + std::to_string(numColumns) +
                                            " fields but found " +
                                            std::to_string(fields.size()));
        }

        Song song;
        song.id = fields[0];
        song.name = fields[1];
        song.artist = fields[2];
        song.language = fields[3];
        song.features.reserve(numFeatures);  // we know the size, so allocate once
        for (size_t i = LABEL_COLUMNS.size(); i < numColumns; i++) {
            song.features.push_back(parseFeature(fields[i], lineNumber));
        }
        songs.push_back(song);
    }
    return songs;
}
