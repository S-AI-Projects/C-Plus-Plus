# R22-T029 — Measure the work done by `Fast_InvSqrt` (math/inv_sqrt.cpp)

## What `Fast_InvSqrt` does (recorded before any change)

- **Signature:** `template <typename T = double, char iterations = 2> T Fast_InvSqrt(T x)`.
- **Result:** an approximation of `1/√x`. It takes the bits of `x` as an integer, subtracts half of them
  from a magic constant to get a first guess (lines 34–40), then runs 1 or 2 Newton steps (lines 42–45).
- **Mutation:** none, since `x` is passed by value. **External dependencies:** none.
- **Supported input:** positive, finite, normal values. Zero, negatives, infinity and denormals aren't
  meaningful for the bit trick.

## Input size and complexity

- **Input size N = the number of calls.** One call does a fixed amount of work that doesn't depend on the
  value or size of `x`: 1 multiply (`x2`), 1 shift and 1 subtraction (the guess), then 4 multiplies and
  1 subtraction for each Newton iteration.
- **Claim from counting operations:** O(1) per call, so O(N) for N calls. This is a fact about the code,
  not something the timings prove. The timings are just consistent with it.

## Experiment (`work/R22-T029/bench.cpp`)

- N ∈ {100 000, 1 000 000, 10 000 000}, with **5 repetitions** per size.
- Each repetition uses **fresh inputs** from a **fixed seed** (`1000003·N + rep`), log-uniform in [1e-3, 1e3].
- **Only the loop of calls is timed** (`std::chrono::steady_clock`). Generating inputs and checking results
  happen outside the timed part. The run excludes process start-up and compiling, because the benchmark
  is built first and timed in-process.
- **Correctness check for every measured value:** `|y − 1/√x| ≤ 5e-6 · (1/√x)` for 2 iterations.
  **0 failures across all (100 000 + 1 000 000 + 10 000 000) × 5 = 55 500 000 values.** Only correctness affects the exit code; no wall-clock threshold is used.
- Raw numbers are in `timings.csv`. A run from the final `check.py`:

| N | median ns/call | range (5 reps) |
|---|---|---|
| 100 000 | 1.35 | 1.31 – 1.43 |
| 1 000 000 | 1.51 | 1.48 – 1.82 |
| 10 000 000 | 2.05 | 1.98 – 3.07 |

**Reading the results:** time per call stays in a narrow band of a few ns while N grows 100×, so total time
grows roughly linearly, as the O(N) count predicts. The small rise at N = 10⁷ shows up in every run.
At that size the two arrays (160 MB) no longer fit in the CPU cache, so memory speed starts to matter.
That's a memory effect, not a property of the algorithm. This ran on a shared cloud machine, so absolute
numbers vary between runs (a second run gave medians of 1.9, 2.1 and 3.5 ns). **This is an observation,
not a worst-case guarantee.**

## Correctness bounds (measured, now in the doc comment)

Over 2 000 000 log-uniform inputs in [1e-3, 1e3], against `1 / std::sqrt(x)`:
- 1 iteration: max relative error **0.175%** (double 0.001751, float 0.001752)
- 2 iterations: max relative error **4.6e-6**

## Regression added to the normal tests

A neighbouring input to the source anchors, worked out by hand: `1/√25 = 0.2`.
- `Fast_InvSqrt<float, 1>(25.0f)` is within `0.2 × 1.8e-3` of 0.2 (actual 0.199689761)
- `Fast_InvSqrt<double, 2>(25.0)` is within `0.2 × 5e-6` of 0.2 (actual 0.1999992774)

The 8 original anchor assertions are unchanged. As a side note, several of their "expected" values (for example
`0.0998449` for `Standard_InvSqrt(100) = 0.1`) are really fast-method approximations, which still fit inside the
loose `1e-3` tolerance.

## Finding: type-punning is undefined behaviour under optimisation

`Fast_InvSqrt` reads the float's bits with `*reinterpret_cast<Tint *>(&y)`. That breaks C++ strict-aliasing
rules. At `-O2`, GCC warns `dereferencing type-punned pointer will break strict-aliasing rules` and
`'y' is used uninitialized`. The results were still correct in this run, but the compiler is allowed to break them.
The benchmark is built with `-fno-strict-aliasing`, which GCC defines for this pattern, and then no warnings
appear. **I didn't change the implementation** because that's outside this task. The standard fix would be
`std::memcpy` or C++20 `std::bit_cast`. The repo's Debug build (no `-O`) shows no warning.

## Command and result

```
python3 work/R22-T029/check.py
```

This runs `cmake -S . -B build-R22-T029 -DCMAKE_BUILD_TYPE=Debug -DUSE_OPENMP=OFF`, then
`cmake --build build-R22-T029 --target inv_sqrt`, then runs `build-R22-T029/math/inv_sqrt`.
After that it runs `c++ -std=c++17 -O2 -fno-strict-aliasing -Wall -Wextra -Wno-return-type work/R22-T029/bench.cpp`
and the benchmark.

Output: `PASS: 11 checks passed` (10 assertions + 1 benchmark correctness pass). Exit code 0.

**Failure check:** I commented out the 2nd Newton iteration, and the new 2-iteration assertion failed
(`check.py` exited with code 1). I then restored the file.

## Tool versions

- g++ (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0
- cmake 3.28.3
- Python 3.13.18
