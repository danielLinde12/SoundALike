# Showcase notes

Things that would look good at the showcase table (Dec 18), collected as we build.
Add to this whenever something would make a good demo, picture, or answer.
Items marked **TODO** need work we haven't done yet.

The showcase is 40% of the grade. Every team member presents, and we must be able
to explain any part of the code.

---

## 1. The 7-song tree (draw it on paper or a slide)

From `tests/test_kd_tree.cpp`, `testSmall2D`. Seven songs with 2 features (x, y).
This is exactly the tree our code builds:

```
                    (4,1)              depth 0: split on x
                  /       \
            (3,6)           (7,5)      depth 1: split on y
           /     \         /     \
       (2,3)   (1,9)   (6,2)   (5,8)   depth 2: leaves
```

How to explain it:
1. At the root, sort by x: 1,2,3,**4**,5,6,7. The median is (4,1). Smaller x goes left, larger goes right.
2. On the left, sort by y: 3,**6**,9. The median is (3,6). Smaller y goes left, larger goes right.
3. Same on the right: y values 2,**5**,8, so (7,5) is the median.
4. Seven songs, height 3: the smallest possible. Picking the median every time keeps it balanced.

Good visual: plot the 7 points on a grid and draw the split lines (a vertical line at x = 4,
then horizontal lines at y = 6 on the left and y = 5 on the right). It shows the tree
chopping the space into boxes.

## 2. How the search skips work

Same 7-song tree, query **(6.5, 3)**. From `testNearestSmall` in `tests/test_kd_tree.cpp`.

1. **(4,1)**, the root: distance² = 2.5² + 2² = 10.25. Best so far: (4,1).
   The query's x (6.5) is bigger than 4, so go **right** first.
2. **(7,5)**: distance² = 0.5² + 2² = 4.25. Better, new best.
   The query's y (3) is smaller than 5, so go **left** first.
3. **(6,2)**: distance² = 0.5² + 1² = 1.25. New best.
4. Back at (7,5): could (5,8) be closer? It's on the other side of the line y = 5,
   which is 2 away, so it's at least 2² = 4 away. 4 > 1.25, so **skip it**.
5. Back at the root: could the left side be closer? Everything there is past the line
   x = 4, which is 2.5 away, so at least 2.5² = 6.25 away. 6.25 > 1.25, so **skip all 3 songs**.

**Answer: (6,2), after checking only 3 of 7 songs.** Brute force always checks all 7.

The key sentence: *"If the split line is already farther than our best song, nothing on
the other side can be closer, so we skip that whole branch."*

### Early numbers (from the k = 1 tests, 5,000 random songs, 1,000 searches)

| Features | Distances computed per search (avg) | Brute force |
| --- | --- | --- |
| 2 | 18 | 5,000 |
| 6 | 214 | 5,000 |
| 10 | 2,352 | 5,000 |

This already shows both stories: a huge win with few features, and the advantage fading
as features grow. The real plots come from the benchmarks.

## 3. Live demo flow (TODO: interface)

1. Type a song name and pick it.
2. Show its 10 most similar songs.
3. Pick a different language (e.g., English song -> Korean results).
4. Bar chart comparing the chosen song's features with a match, showing *why* they matched.
5. Timing panel: k-d tree vs brute force for the same search.

## 4. Benchmark plots (TODO: benchmarks)

- **Time vs number of songs:** brute force grows in a straight line; the k-d tree stays nearly flat.
- **Distances computed vs number of songs:** the same story, without timing noise.
- **Speedup vs number of features (2 to 10):** the tree's advantage shrinks as features grow.
  Being honest about where it fails is a strength, not a weakness.

## 5. Proof the tree gives exact answers (TODO after step 5)

Thousands of random searches, every one returning exactly the same songs as brute force.
This backs up our problem statement: exact answers, fast, on one laptop.

## 6. Proof our tests work

We deliberately broke the build (picked the first song instead of the median) and
**14 tests failed**. Tests that can't fail prove nothing; ours catch real mistakes.

We did the same to the search: swapping which side is searched first broke 5 tests, and
turning off skipping broke the "only 3 of 7" test. Changing `<=` to `<` in the skip test
at first broke **nothing**, which showed a gap: no test had a tie exactly on a split line.
We added one (`testTieAcrossSplit`), and now that change fails too.

## 7. Stories about mistakes we caught

From the mistake tally in `AI_USAGE.md`:
- **The NaN bug:** the range check `value < 0 || value > 1` let `"nan"` through, because
  every comparison with NaN is false. Fixed with `!(value >= 0 && value <= 1)`.
- **The loudness outlier:** one very quiet song (-40 dB) squashed every other song's
  loudness into 0.79-1.0. **TODO:** show before/after once we decide on clipping.
- Total AI mistakes caught, by kind (update from the tally before the showcase).

## 8. Likely questions and short answers

| Question | Answer |
| --- | --- |
| Why pick the median? | Half the songs go each way, so the tree stays balanced: height about log2 n. |
| Why `nth_element` instead of sorting? | It only partly sorts (everything before the middle is <= the median, after is >=). That's all a split needs, in O(n) instead of O(n log n). Building is O(n log n) total. |
| Why `unique_ptr` for children? | Deleting the root frees the whole tree automatically: no memory leaks, no destructor to write. |
| Why squared distance, no square root? | If x < y then sqrt(x) < sqrt(y), so the order is the same, and we skip a sqrt per comparison. |
| What if two songs are the same distance away? | The one with the smaller index wins, in both brute force and the tree, so results match exactly. |
| Why one tree per language? | Language is a label, not a number, so it can't be a coordinate. Searching the Korean tree with an English song's features gives Korean songs that sound like it. |
| Why scale features to 0-1? | Otherwise tempo (60-200) would outweigh everything else (0-1). |
| When does a k-d tree stop helping? | With many features, the search can't skip much, so it gets close to brute force. Our Experiment 2 measures this. |
| How is this different from Spotify? | They use AI on server farms and accept approximate answers. We give exact answers fast on one laptop. |

## 9. Teamwork

Show the GitHub commit history and pull requests: work from both of us, week by week.
