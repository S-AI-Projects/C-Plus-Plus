# R22-T025 — Make verification repeatable for `ConsTree` (range_queries/segtree.cpp)

## What `ConsTree` does (recorded before any change)

- **Inputs:** `const std::vector<int64_t>& arr`, `std::vector<int64_t>* segtree`, `low`, `high` (both included)
  and `pos` (the node to fill; 0 is the root).
- **Result:** returns `void`. It writes the sum of `arr[low..high]` into `segtree[pos]`, filling child
  `2pos+1` (left) and `2pos+2` (right) recursively (lines 40–48).
- **Mutation:** writes only to `*segtree`. `arr` is const and is checked to be unchanged.
- **External dependencies:** **none.** No time, randomness, network, file I/O, UI or global state.
  The function is pure apart from its output parameter, so no seam or mock is needed. The task's
  "inject a dependency" option doesn't apply, and no new parameter was added.

## Hand-worked expected trees

n = 7, `arr = {1..7}`, size 15:
`{28, 10, 18, 3, 7, 11, 7, 1, 2, 3, 4, 5, 6, 0, 0}`. The root is [0,6] = 28, then [0,3] = 10 and [4,6] = 18,
then [0,1] = 3, [2,3] = 7, [4,5] = 11, and the leaf [6] = 7. Slots 13 and 14 are unused for n = 7.

Neighbouring input, n = 8, `arr = {1..8}` (a full tree):
`{36, 10, 26, 3, 7, 11, 15, 1, 2, 3, 4, 5, 6, 7, 8}`.

## Repeatability tests

| Run | State | Expected | Actual |
|---|---|---|---|
| 1 | fresh zero vector | `expected7` | match |
| 2 | another fresh zero vector | `expected7` | match |
| 3 | same vector after building `{7,6,5,4,3,2,1}` | `expected7` | match |
| — | `arr` after the builds | unchanged | unchanged |
| neighbour | fresh, n = 8 | `expected8` | match |
| reuse after larger input | vector holding the n = 8 tree, rebuilt with n = 7 | slots 0–12 = `expected7`, slots 13/14 = 7/8 (stale) | match |

**Finding:** `ConsTree` doesn't clear slots that the new input never reaches. After a larger
build, slots 13 and 14 keep the old leaves 7 and 8. `query` never visits them for n = 7, so I checked
every one of the 28 ranges against a brute-force sum and they're all still correct. This is
documented in the Doxygen comment, not changed.

Every possible range (`lo ≤ hi`) is checked against `brute_sum` (a plain loop) for the fresh n = 7 tree,
the n = 8 tree and the stale reused tree. Each query runs on a copy, so queries can't affect each other.
The source anchors `query(1,5) == 2+3+4+5+6` and, after `update(2,4,+1)`, `== 2+4+5+6+6` still hold.

## Command and result

```
python3 work/R22-T025/check.py
```

This runs `cmake -S . -B build-R22-T025 -DCMAKE_BUILD_TYPE=Debug -DUSE_OPENMP=OFF`, then
`cmake --build build-R22-T025 --target segtree`, then runs `build-R22-T025/range_queries/segtree` with
stdin `3\n0\n0\n` (n = 3, zero-initialise, exit). No network, account or GUI is used.

Output: `PASS: 101 checks passed`. Exit code 0. Clean under `-fsanitize=address,undefined`.

**Failure check:** I temporarily made leaves add instead of assign (`(*segtree)[pos] += arr[low];`),
which is the kind of hidden state this task is about. Fresh builds still passed, but the reused-vector run
failed (`Assertion 'reused == expected7' failed`) and `check.py` exited with code 1. I then restored the file.

## Out-of-scope notes (not changed)

- The interactive `main()` reads numbers with `for (int i = 1; i <= n; i++) std::cin >> arr[i];`, which
  writes `arr[n]`, one past the end, when the user chooses to enter values. The runner avoids that path.
  It deserves its own task.
- The 5 `-Wsign-compare` warnings come from the existing `update()` and `main()`. The original file has
  the same 5; the new code adds none.

## Tool versions

- g++ (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0
- cmake 3.28.3
- Python 3.13.18
