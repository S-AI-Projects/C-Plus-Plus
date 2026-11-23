# R22-T028 — Cover the failure behaviour of `HKGraph` (graph/hopcroft_karp.cpp)

## What `HKGraph` does (recorded before any change)

- **Public API:** `HKGraph()`, `HKGraph(m, n)`, `addEdge(u, v)`, `bfs()`, `dfs(u)` and
  `hopcroftKarpAlgorithm() -> int` (the size of the maximum matching).
- **Supported vertices:** left `u` in `1..m` and right `v` in `1..n` (comments at lines 75–76). Index 0 is
  reserved as `NIL` (line 70) on both sides.
- **Mutation:** `addEdge` appends to `adj`. `hopcroftKarpAlgorithm` rebuilds `pair_u`, `pair_v` and
  `dist` from scratch on every call (lines 100–106), so running it twice gives the same answer.
- **External dependencies:** none. `main()` uses stdin and stdout.

## The contract is total: no exceptions, no error value

Nothing in the class throws or returns a sentinel. The "failure" branches are the
**no augmenting path** results:

| Branch | Source line | Smallest input that reaches it |
|---|---|---|
| `bfs()` returns false on its first call, result 0 | 182 `return (dist[NIL] != INF);` | `HKGraph(0, 0)`, `HKGraph()`, `HKGraph(1, 1)` with no edges |
| `dfs(u)` fails for a free vertex | 215–216 `dist[u] = INF; return false;` | `HKGraph(2, 2)` with edges (1,1), (2,1): left 1 and 2 both want right 1 |

**Showing the input reaches line 215:** I built a temporary driver that runs only the
(1,1), (2,1) graph with `--coverage`. gcov showed line 215 ran **once** (for `dfs(2)`)
and line 182 ended the loop. Result: 1.

## Tests added (`failure_path_tests()`)

| Case | Expected | Actual |
|---|---|---|
| `HKGraph(0, 0)` | 0 | 0 |
| `HKGraph()` (default) | 0 | 0 |
| `HKGraph(1, 1)`, no edges | 0 | 0 |
| (1,1), (2,1) | 1 | 1 |
| same graph, run again (state still valid) | 1 | 1 |
| then `addEdge(2, 2)`, a normal successful call | 2 | 2 |
| neighbouring input: source test 2 without edge (4,2) | 3 | 3 |

**Working for the neighbouring input:** edges 1–1, 1–3, 2–3, 3–4, 4–3. Left 2 and left 4 can only use
right 3, so at most one of them gets matched. 1–1, 3–4 and one of {2–3, 4–3} gives 3.

## Defect found in the existing anchor tests

All three original self-tests asserted `expected_res = 0`, but the algorithm returns **1, 4 and 2**.
With `assert` switched on (the repo's own `cmake -DCMAKE_BUILD_TYPE=Debug`), the original program aborts:

```
hopcroft_karp: graph/hopcroft_karp.cpp:266: void tests(): Assertion `res1 == expected_res1' failed.
```

It only looks like it passes in builds that define `NDEBUG`. The algorithm is right and the expected
values were wrong:

| Test | Graph | Correct answer | Change |
|---|---|---|---|
| 1 | (0,1), (1,4) | **1**. (0,1) starts at the dummy vertex 0, which is never visited | `0 → 1` |
| 2 | 1–1, 1–3, 2–3, 3–4, 4–3, 4–2 | **4**, a perfect matching 1–1, 2–3, 3–4, 4–2 | `0 → 4` |
| 3 | (0,1), (1,4), (1,5), (5,0) | **2 as currently computed**, see below | `0 → 2` |

**Test 3 uses an unsupported input.** Edge (5, 0) ends at right vertex 0, the reserved `NIL` index. The
implementation still matches 5 with "vertex 0", so it reports 2. Over supported vertices only, the
answer would be 1. As the task rules say, I didn't add a rejection rule. I recorded 2 as the current
behaviour, with a comment saying it isn't a valid matching. **This one is worth raising with your
instructor:** they may want the test input fixed instead.

## Command and result

```
python3 work/R22-T028/check.py
```

This runs `cmake -S . -B build-R22-T028 -DCMAKE_BUILD_TYPE=Debug -DUSE_OPENMP=OFF`, then
`cmake --build build-R22-T028 --target hopcroft_karp`, then runs `build-R22-T028/graph/hopcroft_karp`
with stdin `1 1 0` and checks for `Maximum matching is 0`.

Output: `PASS: 8 checks passed` (7 assertions + 1 program output check). Exit code 0.
Clean under `-fsanitize=address,undefined`. The original file aborts under the same command.

**Mutant checks (temporary edits, reverted afterwards):**
- `dfs` returning `true` instead of `false` at line 216 means a failed search claims a match. The assertions
  fail and `check.py` exits with code 1.
- Deleting `dist[u] = INF` at line 215 does **not** change any result. That line only stops failed vertices
  being searched again in the same phase, which is a speed optimisation, so no correctness test can
  catch it. Noted here so the coverage claim isn't overstated.

## Tool versions

- g++ (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0
- cmake 3.28.3
- Python 3.13.18
- gcov (same version as g++), used once for the reachability check
