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

## 2. How the search skips work (TODO after step 3)

Walk through one search on the 7-song tree: which nodes it visits, and which branch it
skips because the split line is farther away than the best song found so far.
This is the core idea and the most likely question. Show "visited 3 of 7" vs brute force's 7.

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
