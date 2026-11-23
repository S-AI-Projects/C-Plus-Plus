# R22-T026 — Simplify one responsibility in Gaussian elimination (numerical_methods/gaussian_elimination.cpp)

## What the program does (recorded before any change)

All the logic was inside `main()`:
1. It reads `mat_size` (n), then an **augmented** n × (n+1) matrix from stdin.
2. **Forward elimination** (old lines 33–40): makes the matrix upper-triangular. There is no row swapping.
3. It prints the eliminated matrix, then does **back substitution** (old lines 53–65), printing `x(n-1)` down to `x0`.

- **Public interface:** the stdin/stdout format of the program. No functions were exposed.
- **Mutation:** `mat` is changed in place. **External dependencies:** stdin and stdout only.

## Preconditions and the source lines that set them

| Precondition | Source line (original) |
|---|---|
| `mat_size >= 1` | `std::cin >> mat_size;` (line 13), which sizes every `new[]` on line 16 |
| augmented matrix, n + 1 columns per row | the input loop `for (j = 0; j <= mat_size; j++)` (line 26) |
| every pivot non-zero | the division `mat[i + 1][step] / mat[step][step]` (line 35) |

## The refactor: one responsibility

Forward elimination moved out of `main()` into
`void forward_elimination(double **mat, int mat_size)`.

**What it makes clearer:** the row being eliminated is named directly as `row` (running `step+1 .. n-1`).
The old loop used `i` running `step .. n-2` and wrote `mat[i + 1]` in four places, so you had to work out
the offset in your head. The Doxygen comment now states the operation (subtract `a` times the pivot row
to zero out `mat[row][step]`) and the no-row-swapping precondition.

The algorithm, the order of operations, the floating-point expressions, the output and the arguments
are all unchanged. `main()` now calls `forward_elimination(mat, mat_size)`, and the unused `step`
variable was removed. This isn't a reformat of the file or an optimisation.

## Tests (inputs and expected outputs worked out by hand before editing)

**Unit checks on `forward_elimination` (in the source):**

| Input | Expected after elimination |
|---|---|
| smallest: `[2 \| 6]` (2x = 6) | unchanged `[2 \| 6]` |
| `2x + y = 5`, `x − y = 1` | row 1: `a = 1/2` → `[0, −1.5 \| −1.5]` |
| `x+y+z=6`, `2y+5z=−4`, `2x+5y−z=27` | row 2 after step 0: `[0, 3, −3 \| 15]`; after step 1 (`a = 1.5`): `[0, 0, −10.5 \| 21]` |

All of these values are exact in binary floating point, so the checks use `==`.

**Before/after program output (`check.py`):** for each input below, the refactored program **and the
original file at upstream commit `fdcc29b`** must both print exactly the hand-worked text:

| stdin | Expected solution lines |
|---|---|
| `1 / 2 6` | `x0= 3` |
| `2 / 2 1 5 / 1 -1 1` | `x1= 1`, `x0= 2` |
| `3 / 1 1 1 6 / 0 2 5 -4 / 2 5 -1 27` | `x2= -2`, `x1= 3`, `x0= 5` |

The full expected text, including the printed matrix, is in `check.py`. Both versions' actual output
is in `last_run.txt`.

## Command and result

```
python3 work/R22-T026/check.py
```

This runs `cmake -S . -B build-R22-T026 -DCMAKE_BUILD_TYPE=Debug -DUSE_OPENMP=OFF`, then
`cmake --build build-R22-T026 --target gaussian_elimination`, then runs the binary. After that, it
compiles `git show fdcc29b…:numerical_methods/gaussian_elimination.cpp` to a temp folder and
compares the two outputs for the 3 inputs.

Output: `PASS: 12 checks passed`. That's 6 unit assertions plus 6 output comparisons: 3 inputs × (after, before).
Exit code 0. Clean under `-fsanitize=address,undefined`.

**Failure check:** I temporarily changed the column loop to `j < mat_size`, so the right-hand side was skipped.
The unit assertion for row 1 failed and `check.py` exited with code 1. I then restored the file.

## Tool versions

- g++ (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0
- cmake 3.28.3
- Python 3.13.18
