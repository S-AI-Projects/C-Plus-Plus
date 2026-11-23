# R22-T030 — Strengthen the tests of `maxKnapsackValue` (dynamic_programming/0_1_knapsack.cpp)

## What `maxKnapsackValue` does (recorded before any change)

- **Signature:** `template <size_t n> int maxKnapsackValue(const int capacity, const std::array<int, n>& weight, const std::array<int, n>& value)`.
- **Result:** the highest total value from a subset of items whose total weight is ≤ `capacity`, with each
  item used at most once. It fills a `(n+1) × (capacity+1)` table bottom-up (lines 53–84) and returns
  `maxValue[n][capacity]`.
- **Mutation:** none, because the inputs are const references. The test confirms they're unchanged.
  **External dependencies:** none.

## Mutation experiment (temporary copies only)

I tried ten one-line mutants against the **original** tests (anchors `max_value1 == 220` and
`max_value2 == 36`):

| Mutant | Original tests |
|---|---|
| `weight[i-1] <= j` → `<` (boundary comparison) | killed |
| reuse items: `maxValue[i-1][…]` → `maxValue[i][…]` | killed |
| `j < capacity + 1` → `j < capacity` | killed |
| `i < items + 1` → `i < items` | killed |
| `std::max` → `std::min` | killed |
| wrong value index | killed |
| **too-heavy branch: `maxValue[i][j] = maxValue[i-1][j]` → `= 0`** | **survived** |
| too-heavy branch: `… = maxValue[i-1][j-1]` | survived |
| `profit2 = maxValue[i-1][j]` → `[j-1]` | survived |
| `if (i == 0 \|\| j == 0)` → `if (i == 0)` | survived (an **equivalent** mutant: with positive weights, the `j == 0` row already comes out as 0 through the else branch) |

**Chosen mutant:** in the "item doesn't fit" branch (line 81), resetting the cell to 0 instead of keeping
the best value of the earlier items. It's a realistic slip ("if it doesn't fit, it's worth 0"), and both
original tests miss it because in each of them the **last** item fits the full capacity.

## Smallest input that tells them apart

The mutant only goes wrong when the **last** item can't fit at some capacity where earlier items had value.
With n = 1 both versions give 0, so the smallest case needs n = 2:

- `weight {1, 2}`, `value {10, 20}`, `capacity 1`. Only item 1 fits, so the **correct answer is 10**.
  The original gives 10. The mutant gives 0, because item 2 (weight 2 > 1) resets the final row.

**Neighbouring input to anchor test 1:** add an item that's too heavy, `weight {10, 20, 30, 60}`,
`value {60, 100, 120, 500}`, `capacity 50`. Item 4 can't fit, so the answer must stay
**220** (items 2 + 3: weight 50, value 100 + 120). The original gives 220; the mutant gives 0.

## Tests added (Test 3 and Test 4 in `test()`)

| Assertion | Expected | Original | Mutant |
|---|---|---|---|
| `max_value3 == expected_max_value3` (heavy 4th item) | 220 | 220 ✔ | 0 ✘ (first failure) |
| fresh equivalent input gives the same result | 220 | 220 ✔ | — |
| inputs unchanged after the call | true | true ✔ | — |
| `max_value4 == expected_max_value4` (smallest case) | 10 | 10 ✔ | 0 ✘ |

The protected condition is also documented in a comment on line 81's branch.
**The final source contains no mutation and no test-only branch.** The mutant is built only in a temp folder
by `check.py`, which requires it to fail **the named assertion** `max_value3 == expected_max_value3`.
The mutant `… = maxValue[i-1][j-1]` is also killed by the new Test 3.

**Remaining gap, stated honestly:** the `profit2 → maxValue[i-1][j-1]` mutant still survives. For example,
`weight {1, 1}`, `value {10, 1}`, `capacity 1` would catch it (correct 10, mutant 1). I left it out
to keep this PR to one protected condition.

## Command and result

```
python3 work/R22-T030/check.py
```

This runs `cmake -S . -B build-R22-T030 -DCMAKE_BUILD_TYPE=Debug -DUSE_OPENMP=OFF`, then
`cmake --build build-R22-T030 --target 0_1_knapsack`, then runs
`build-R22-T030/dynamic_programming/0_1_knapsack`, then compiles and runs the mutant.

Output: `PASS: 7 checks passed` (6 new assertions + 1 mutant rejected by the named assertion). Exit code 0.
Clean under `-fsanitize=address,undefined`. Repeated runs give identical output.

**Warnings:** the existing `-Wsign-compare` warnings on lines 58, 59 and 64 appear once more because the
template is now also built for `n = 2`. The new code adds no warnings of its own.

## Tool versions

- g++ (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0
- cmake 3.28.3
- Python 3.13.18
