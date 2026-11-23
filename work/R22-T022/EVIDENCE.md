# R22-T022 — Define the supported boundaries of `minSwaps` (sorting/swap_sort.cpp)

## What `minSwaps` does (recorded before any change)

- **Inputs:** `int arr[]` and `int n`, the number of leading elements to use.
- **Result:** `int`, the number of swaps needed to sort those `n` values in ascending order. It counts
  the cycles in the permutation from original index to sorted index; each cycle of length k costs k − 1 swaps.
- **Mutation:** `arr` is only read. Its values are copied into a heap array `arrPos` (line 13), and
  that copy is what gets sorted (line 22). The tests confirm `arr` is unchanged.
- **External dependencies:** none (no I/O inside the function).
- **Interface note:** the function returns a count, not a sorted array. That's why the issue's
  "expected ascending outputs" become expected swap counts here.

## Boundary table

| Partition | Input | Expected | Actual | Source contract |
|---|---|---|---|---|
| smallest supported (empty) | `{}`, n=0 | 0 | 0 | line 13 `new pair[0]` is valid; loops at 14/32 do not run |
| smallest non-empty | `{5}`, n=1 | 0 | 0 | line 35: element already at its sorted position |
| interior, already ordered | `[1, 2, 3]` | 0 | 0 | every `arrPos[i].second == i` (line 35) |
| interior | `[3, 1, 2]` | 2 | 2 | one 3-cycle 0→1→2→0, 3 − 1 = 2 (line 52) |
| interior, reversed | `[3, 2, 1]` | 1 | 1 | one 2-cycle (0 2); middle value fixed |
| interior, program example | `[6, 7, 8, 1, 2, 3, 9, 12]` | 3 | 3 | three 2-cycles (0 3)(1 4)(2 5) |
| prefix only | `[3, 1, 2]`, n=2 | 1 | 1 | lines 14–17 read only `arr[0..n-1]` |
| repeated values (all equal) | `[2, 2, 2]` | 0 | 0 | ties sort by index (`std::pair` comparison), so nothing moves |
| repeated values (boundary) | `[2, 2, 1]` | 2 | 2 | ties broken by index give the 3-cycle 0→2→1→0 |
| **unsupported** | n < 0 | not tested | — | line 13 `new[n]` with negative n throws `std::bad_array_new_length` |

## Finding: the result is not minimal when values repeat

For `[2, 2, 1]` the function returns **2**, but one swap (index 0 with index 2) already gives
`[1, 2, 2]`. The cycle method only gives the true minimum when all values are distinct. With repeats,
ties are broken by original index, which fixes one particular permutation, so the answer is a valid swap
count but can be higher than the minimum. Finding the true minimum with repeated values is a different,
harder problem.

Per the task rules, I didn't add a rejection rule. The behaviour is kept and now documented in the
function's Doxygen comment. The test asserts the current value, 2, so any change to it shows up.

## Command and result

```
python3 work/R22-T022/check.py
```

This runs `cmake -S . -B build-R22-T022 -DCMAKE_BUILD_TYPE=Debug -DUSE_OPENMP=OFF`, then
`cmake --build build-R22-T022 --target swap_sort`, then runs `build-R22-T022/sorting/swap_sort`.

Output: `PASS: 18 checks passed`. That's 9 cases, each checking the count and that the input is unchanged. Exit code 0.

**Failure check:** I temporarily changed the `[2, 2, 1]` expectation to 1. The assertion failed and
`check.py` exited with code 1. I then restored the file. The program still prints `3` for its
original example, so `main()`'s existing behaviour is kept.

## Tool versions

- g++ (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0
- cmake 3.28.3
- Python 3.13.18
