# Cleaned dataset format

The Python cleaning script writes this file and the C++ loader (`src/`) reads it.
**If you change the format, update this document and the loader together.**

## Columns, in order

| #   | Column        | Type   | Notes                                                                          |
| --- | ------------- | ------ | ------------------------------------------------------------------------------ |
| 1   | `track_id`    | text   | Spotify track ID. Unique per row; used to tell apart songs with the same name. |
| 2   | `track_name`  | text   | Song title.                                                                    |
| 3   | `artist_name` | text   | Artist (if there are several, keep them in one field).                         |
| 4   | `language`    | text   | One of: `English`, `Hindi`, `Tamil`, `Telugu`, `Malayalam`, `Korean`.          |
| 5+  | features      | number | One column per audio feature, every value scaled to **0-1**.                   |

The main features, in this order:

```
danceability, energy, valence, acousticness, tempo, loudness
```

**Feature columns are not fixed.** Every column after `language` is treated as a
feature, so the header row decides how many dimensions there are. That lets us
add more features (up to 10) for Experiment 2 without changing the C++ code.

## Rules

- The first row is a header with exactly the column names above.
- The file uses commas as separators and is UTF-8 (needed for Hindi, Tamil, Korean, etc.).
- Text fields that contain a comma or a double quote are wrapped in double quotes,
  and a quote inside a field is written twice (`""`). Python's `csv` module does
  this by default, and the C++ loader must handle it.
- There are no missing values: the cleaning script drops any row with a missing field.
- All feature values are already scaled to 0-1 by the cleaning script. The C++ code
  does no scaling.

## Example

See `sample_songs.csv`, a small hand-made file for testing the loader. Its songs
and numbers are made up.

```
track_id,track_name,artist_name,language,danceability,energy,valence,acousticness,tempo,loudness
t001,Morning Run,Test Artist A,English,0.80,0.90,0.70,0.10,0.75,0.85
t004,"Rain, Again",Test Artist B,Korean,0.30,0.20,0.25,0.90,0.30,0.35
```
