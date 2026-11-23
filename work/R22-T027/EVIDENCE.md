# R22-T027 — Publish a runnable example for `BinaryAddition` (greedy_algorithms/binary_addition.cpp)

## What `BinaryAddition` does (recorded before any change)

- **Public API:** `std::string addBinary(const std::string& a, const std::string& b)`.
- **Result:** the binary sum as a string. It returns `""` if either input has a character other than
  `'0'` or `'1'` (line 41). That's the only failure sentinel.
- **Mutation:** none, because the inputs are const references. **External dependencies:** none.
- **How it works:** it reads bits right to left (lines 51–61), adds `bitA + bitB + carry`, writes `sum % 2`
  and carries `sum / 2`. A missing bit in the shorter string counts as 0. After the loop, any remaining carry
  becomes a leading `'1'` (lines 62–64), and the reversed string is returned.

## The documented example (added to the file's Doxygen `@details`)

```cpp
greedy_algorithms::BinaryAddition adder;
std::cout << adder.addBinary("1011", "110");  // prints 10001
```

11 + 6 = 17. This is a neighbouring input next to the source anchor `"101" + "11"`. It adds one more bit
and makes `b` the shorter string. Hand-worked trace (also in the doc comment):

| i | bitA | bitB | carry in | sum | bit | carry out | result so far |
|---|---|---|---|---|---|---|---|
| 0 | 1 | 0 | 0 | 1 | 1 | 0 | "1" |
| 1 | 1 | 1 | 0 | 2 | 0 | 1 | "10" |
| 2 | 0 | 1 | 1 | 2 | 0 | 1 | "100" |
| 3 | 1 | 0 (missing) | 1 | 2 | 0 | 1 | "1000" |

**The decisive step:** when the loop ends, the carry is still 1, so `if (carry)` appends `'1'`, and
`"10001"` comes out after the reverse. Without that step the result would be `"0001"`.

## Comparing the trace with a run

Each intermediate state is checked through the real public API. After k bits, the state is the
sum of the last k bits of each input:

| k | call | expected | what it shows |
|---|---|---|---|
| 1 | `addBinary("1", "0")` | `"1"` | bit 1, carry 0 |
| 2 | `addBinary("11", "10")` | `"101"` | bits `01`, carry 1 |
| 3 | `addBinary("011", "110")` | `"1001"` | bits `001`, carry 1 |
| 4 | `addBinary("1011", "0110")` | `"10001"` | the final carry becomes the leading 1 |

All of them match. Also asserted: the three source anchors (`10111`, `11110`, `1000`) and another
neighbour, `"101" + "111" = "1100"` (5 + 7 = 12).

## How the check ties the doc to the run

1. The test runs the **exact line** from the `@code` block with `std::cout` captured, and asserts `"10001"`.
2. `check.py` reads the `@code` block from the source, pulls out the call and its `// prints` value, and
   checks three things: the test runs that same call, the binary actually printed that value, and the value
   equals `int(a, 2) + int(b, 2)`.

## Command and result

```
python3 work/R22-T027/check.py
```

This runs `cmake -S . -B build-R22-T027 -DCMAKE_BUILD_TYPE=Debug -DUSE_OPENMP=OFF`, then
`cmake --build build-R22-T027 --target binary_addition`, then runs
`build-R22-T027/greedy_algorithms/binary_addition` and the documentation checks.

Output: `PASS: 10 checks passed` (9 assertions + 1 documentation check). Exit code 0.
Clean under `-fsanitize=address,undefined`.

**Failure checks:**
- I changed the doc comment to `// prints 1001`, and `check.py` failed with code 1: the doc no longer matches
  what's executed.
- I replaced `if (carry)` with `if (false)`, and both the existing and new assertions failed with code 1.

Both changes were reverted afterwards.

## Notes (not changed)

- The 2 `-Wsign-compare` warnings (lines 53–54, `int i` compared with `size()`) were already there.
- `addBinary("", "")` returns `""`, the same value as the error sentinel, because an empty string passes
  validation. I left it alone because it's outside this task.

## Tool versions

- g++ (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0
- cmake 3.28.3
- Python 3.13.18
