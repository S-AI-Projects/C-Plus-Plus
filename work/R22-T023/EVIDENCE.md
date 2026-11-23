# R22-T023 — Cross-check `SlowSort` (sorting/slow_sort.cpp)

## What `SlowSort` does (recorded before any change)

- **Inputs:** `int a[]` and two indices, `i` and `j`. **`j` is included.** The function sorts `a[i..j]`.
- **Result:** returns `void` and sorts **in place**, ascending. Elements outside `a[i..j]` are not touched.
- **External dependencies:** none. The sample `main()` reads from stdin and writes to stdout.

## Oracle

`std::sort` on a copy, which is introsort: a different method that never calls `SlowSort`.
On top of that, every case checks `std::is_sorted` (the output is ordered) and
`std::is_permutation` against the input (the same values with the same multiplicities).

## Fixed expected-result table (worked out by hand)

| Input | Expected | Actual |
|---|---|---|
| [3, 1, 2] | [1, 2, 3] | [1, 2, 3] |
| [2, 2, 1] | [1, 2, 2] | [1, 2, 2] |
| [1, 2, 3] | [1, 2, 3] | [1, 2, 3] |
| [3, 2, 1] | [1, 2, 3] | [1, 2, 3] |
| [7] | [7] | [7] |
| [] (smallest, `j = -1`) | [] | [] |

## Enumeration

I ran every sequence of length 0 to 4 over the values {1, 2, 3}: 1 + 3 + 9 + 27 + 81 = **121 inputs**.
All of them matched the oracle and kept their multiplicities.

## Mismatch found and reduced: the sample `main()` read and wrote past the end

The original `main()` called `SlowSort(arr, 0, size)`. Since `j` is included, that also sorts
`arr[size]`, one element past the end of the heap array.

- **Smallest reproduction:** `size = 3`, input `3 1 2`. AddressSanitizer on the original file:
  `heap-buffer-overflow ... READ of size 4 ... in SlowSort(int*, int, int) slow_sort.cpp:21`.
- **Regression shown safely in the tests:** with a 4-slot buffer `{3, 1, 2, -100}`, the old call
  `SlowSort(a, 0, 3)` gives `{-100, 1, 2, 3}`. A value that wasn't in the 3-element input ends up in
  the result, and `3` is pushed past the end. The corrected call `SlowSort(a, 0, 2)` gives
  `{1, 2, 3, -100}`, and the extra slot is left alone.
- **Fix:** `main()` now calls `SlowSort(arr, 0, size - 1)`. `SlowSort` itself and its interface are unchanged.
  The fixed file runs clean under `-fsanitize=address,undefined`.

## Mutation contract

`{9, 3, 1, 2, 0}` sorted with `i = 1, j = 3` gives `{9, 1, 2, 3, 0}`. Only the chosen range changes.

## Command and result

```
python3 work/R22-T023/check.py
```

This runs `cmake -S . -B build-R22-T023 -DCMAKE_BUILD_TYPE=Debug -DUSE_OPENMP=OFF`, then
`cmake --build build-R22-T023 --target slow_sort`, then runs `build-R22-T023/sorting/slow_sort`
with stdin `3\n3 1 2\n`.

Output: `PASS: 385 checks passed`. That's 384 assertions in the test function plus 1 check that the sample
program prints `1 2 3`. Exit code 0.

**Failure check:** I temporarily flipped the comparison at line 21 (`a[j] > a[m]`). The `is_sorted`
assertion failed and `check.py` exited with code 1. I then restored the file.

## Tool versions

- g++ (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0
- cmake 3.28.3
- Python 3.13.18
