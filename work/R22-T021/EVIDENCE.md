# R22-T021 — Lock down the contract of `print` (backtracking/magic_sequence.cpp)

## What `print` does (recorded before any change)

- **Input:** `const sequence_t& s` (`std::vector<unsigned int>`), passed by const reference.
- **Result:** returns `void`. Its only effect is writing to `std::cout`: each value followed by one
  space, then `std::endl` (a newline plus a flush).
- **Mutation:** none possible through the const reference. The tests check this separately anyway.
- **External dependency:** the global `std::cout` stream. The tests point it at a buffer to read what was printed.
- **No validation:** `print` writes any sequence, magic or not.

## Inputs and expected results (worked out by hand before running)

| Case | Input | Expected text | Actual | Result |
|---|---|---|---|---|
| ordinary (fixture, n=10) | `6 2 1 0 0 0 1 0 0 0` | `"6 2 1 0 0 0 1 0 0 0 \n"` | same | pass |
| non-magic fixture | `5 2 1 0 0 0 1 0 0 0` | `"5 2 1 0 0 0 1 0 0 0 \n"` | same | pass |
| neighbouring input (n=11) | `7 2 1 0 0 0 0 1 0 0 0` | `"7 2 1 0 0 0 0 1 0 0 0 \n"` | same | pass |
| smallest (empty) | `{}` | `"\n"` | same | pass |
| single value | `0` | `"0 \n"` | same | pass |
| repeated values | `3 3 3 3` | `"3 3 3 3 \n"` | same | pass |

**Working for the neighbouring input (n=11):** in `7 2 1 0 0 0 0 1 0 0 0`, the value 0 appears 7 times
(positions 3, 4, 5, 6, 8, 9, 10), 1 appears 2 times, 2 appears 1 time and 7 appears 1 time. Every other
value appears 0 times, so it is magic. The test asserts `is_magic(...) == true` for it as well.

Each `print` case asserts two things: the exact text matches, and the input is unchanged. That gives 12 checks.
The 2 original `is_magic` fixture anchors and the `is_magic` check for the n=11 input add 3 more,
for **15 checks** in total.

## Baseline

Every hand-worked expected value matched the real output. No defect was found, so `print` and the
public interface are unchanged. Only the tests were added.

## Command and result

```
python3 work/R22-T021/check.py
```

This runs:

```
cmake -S . -B build-R22-T021 -DCMAKE_BUILD_TYPE=Debug -DUSE_OPENMP=OFF
cmake --build build-R22-T021 --target magic_sequence
build-R22-T021/backtracking/magic_sequence
```

Output: `PASS: 15 checks passed`, exit code 0. The Debug build keeps `assert` switched on.
The file also builds without warnings using the repository flags (`-std=c++17 -Wall -Wextra -Wno-register -Werror=vla`).

**Failure check:** I temporarily changed the repeated-values expectation to `"3 3 3 \n"`. The assertion
failed, the binary aborted, and `check.py` reported `FAIL` with exit code 1. I then reverted the change.

## Tool versions

- g++ (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0
- cmake 3.28.3
- Python 3.13.18
