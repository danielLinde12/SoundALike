#!/usr/bin/env python3
"""
clean_data.py - turn the raw Kaggle dataset into the format in data/README.md.

Input:  Kaggle "Spotify Tracks Dataset (Updated Every Week)" (gauthamvijayaraj)
Output: a cleaned CSV with these columns, in this order:
        track_id, track_name, artist_name, language,
        danceability, energy, valence, acousticness, tempo, loudness
        (every feature scaled to 0-1)

Usage (from the repo root):
    python data/clean_data.py path/to/raw.csv data/songs.csv
"""

import argparse
import sys

import pandas as pd

# ---------------------------------------------------------------------------
# Output format (must match data/README.md and the C++ loader)
# ---------------------------------------------------------------------------
LANGUAGES = ["English", "Hindi", "Tamil", "Telugu", "Malayalam", "Korean"]
FEATURES = ["danceability", "energy", "valence", "acousticness", "tempo", "loudness"]

# The raw file might name a column slightly differently than we do.
# For each output column, list the raw names we are willing to accept
# (checked in order, ignoring upper/lower case).
ALIASES = {
    "track_id":    ["track_id", "id", "spotify_id"],
    "track_name":  ["track_name", "name", "title", "song_name"],
    "artist_name": ["artist_name", "artists", "artist", "artist_names"],
    "language":    ["language", "lang"],
    **{f: [f] for f in FEATURES},
}


def find_columns(df):
    """Map each output column to the matching raw column. Exit with a
    helpful message if one can't be found."""
    lower = {c.lower().strip(): c for c in df.columns}
    mapping = {}
    for out_name, candidates in ALIASES.items():
        for cand in candidates:
            if cand in lower:
                mapping[out_name] = lower[cand]
                break
        else:
            sys.exit(
                f"Could not find a column for '{out_name}'.\n"
                f"Looked for: {candidates}\n"
                f"The raw file has: {list(df.columns)}\n"
                f"Add the right name to ALIASES in this script."
            )
    return mapping


def clean(raw):
    start = len(raw)

    # 1. Keep only the columns we need, renamed to our format
    mapping = find_columns(raw)
    df = raw[list(mapping.values())].copy()
    df.columns = list(mapping.keys())          # same order as ALIASES

    # 2. Tidy the text columns (strip spaces, treat "" as missing)
    for col in ["track_id", "track_name", "artist_name", "language"]:
        df[col] = df[col].astype("string").str.strip()
        df[col] = df[col].replace("", pd.NA)

    # 3. Features must be numbers; anything unreadable becomes missing
    for col in FEATURES:
        df[col] = pd.to_numeric(df[col], errors="coerce")

    # 4. Drop rows with ANY missing value (the spec says there are none)
    df = df.dropna()

    # 5. Keep only the six languages we support
    df = df[df["language"].isin(LANGUAGES)]

    # 6. Drop bad data: a tempo of 0 means the analysis failed
    df = df[df["tempo"] > 0]

    # 7. Remove duplicates. track_id must be unique per row. We also drop
    #    the same song+artist+language appearing under different ids.
    df = df.drop_duplicates(subset="track_id")
    df = df.drop_duplicates(subset=["track_name", "artist_name", "language"])

    # 8. Scale every feature to 0-1 with min-max scaling:
    #        scaled = (x - min) / (max - min)
    #    Done AFTER cleaning, so dropped rows don't affect min and max.
    for col in FEATURES:
        lo, hi = df[col].min(), df[col].max()
        if hi == lo:
            sys.exit(f"Feature '{col}' has the same value in every row.")
        df[col] = ((df[col] - lo) / (hi - lo)).round(4)

    # 9. Put columns in the exact order the C++ loader expects
    df = df[["track_id", "track_name", "artist_name", "language"] + FEATURES]

    print(f"Rows in:  {start:,}")
    print(f"Rows out: {len(df):,}  ({start - len(df):,} dropped)")
    print("Songs per language:")
    print(df["language"].value_counts().to_string())
    return df.reset_index(drop=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("input", help="path to the raw Kaggle CSV")
    parser.add_argument("output", help="where to write the cleaned CSV")
    args = parser.parse_args()

    raw = pd.read_csv(args.input)
    cleaned = clean(raw)

    # pandas quotes fields containing commas/quotes automatically, and
    # utf-8 keeps Hindi, Tamil, Korean, etc. intact.
    cleaned.to_csv(args.output, index=False, encoding="utf-8")
    print(f"Wrote {args.output}")


if __name__ == "__main__":
    main()
