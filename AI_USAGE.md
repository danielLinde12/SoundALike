# AI usage log

Notes on where we used AI tools, what they got wrong, and what we changed or rejected.
This feeds the "AI usage" section of REPORT.md.

| Date | Who | Tool | What it helped with | What we checked / changed |
|------|-----|------|---------------------|---------------------------|
| 2026-11-05 | Daniel | Claude Code | Drafted the CSV format (`data/README.md`), a sample CSV, and the `Song` struct (`src/song.h`). | _To fill in after review with Kevin (e.g., column names checked against the real Kaggle file)._ |
| 2026-11-05 | Daniel | Claude Code | Wrote the CSV loader (`src/csv_loader.*`) and its tests. | Its first version had a bug: the 0–1 range check (`value < 0 \|\| value > 1`) let `"nan"` through, because comparisons with NaN are always false. Caught in review and changed to `!(value >= 0 && value <= 1)`. |
| 2026-11-05 | Daniel | Claude Code | Wrote brute-force k-nearest search (`src/brute_force.*`) and hand-checkable tests. | Ties are broken by smaller index so brute force and the k-d tree have one exact answer to compare. _To review: confirm we want this rule._ |
| 2026-11-08 | Kevin | Claude (chat) | Wrote the data cleaning script (data/clean_data.py): keeps the columns from data/README.md, drops missing/duplicate/bad rows, keeps the six languages, and min-max scales the six features to 0–1.|Its first draft guessed the output format before reading data/README.md (wrong column names, no language column), so it was rewritten to match the real spec. It was only tested on a small made-up CSV, not the real Kaggle file, and the raw column names are guesses (see ALIASES in the script). To do: run it on the real dataset and fix ALIASES if needed. Decide whether to clip loudness outliers before scaling, since one very quiet track can squash the rest toward 1.|

## Mistake tally

Every mistake an AI tool made that we caught, so the report can say how often AI was wrong and how.
Add a row whenever review, a test, or a teammate finds one. "Found by" is how we noticed it.

**Kinds of mistakes:** Bug (code did the wrong thing) · Wrong assumption (guessed something it
should have checked) · Missing case (didn't handle an input) · Design (worked, but a poor choice)

| # | Date | Where | Kind | What the AI got wrong | Found by | Fix |
|---|------|-------|------|------------------------|----------|-----|
| 1 | 2026-11-05 | `src/csv_loader.cpp` | Bug | The 0-1 range check (`value < 0 \|\| value > 1`) let `"nan"` through, because comparisons with NaN are always false. | Code review | Changed to `!(value >= 0 && value <= 1)`. |
| 2 | 2026-11-08 | `data/clean_data.py` | Wrong assumption | First draft invented its own output format (wrong column names, no language column) instead of reading `data/README.md`. | Kevin, reading the output | Rewritten to match the spec. |
| 3 | 2026-11-08 | `data/clean_data.py` | Wrong assumption | Raw Kaggle column names are guesses (`ALIASES`), never checked against the real file. | Kevin | _Open: run on the real dataset._ |
| 4 | 2026-11-12 | `data/clean_data.py` | Missing case | Language filter is case-sensitive, so a song labeled `english` is silently dropped. | Review test with a fake raw file | _Open: add `.str.title()` before the filter._ |
| 5 | 2026-11-12 | `data/clean_data.py` | Design | Plain min-max scaling lets one outlier squash a feature: a single -40 dB song pushed every other song's loudness into 0.79-1.0. | Kevin flagged it; review test confirmed it | _Open: decide on clipping to the 1st-99th percentile._ |

**Running totals** (update when adding rows): 5 mistakes. Bug 1 · Wrong assumption 2 · Missing case 1 · Design 1. Still open: 3.
