# Task R22-T037 Notes — sorting/quick_sort_3.cpp, `partition3`

All line numbers refer to `sorting/quick_sort_3.cpp` on `master` (before this change).

## Interface

| Question | Answer | Line |
|---|---|---|
| Exact signature | `template <typename T> void partition3(std::vector<T> *arr, int32_t low, int32_t high, int32_t *i, int32_t *j)` | 55–57 |
| Result | `void`; it reports results only by reordering `*arr` and writing `*i` and `*j` | 56, 63–64, 81–82 |
| Mutation | reorders `(*arr)[low..high]` in place with `std::swap` (lines 61, 72, 76); writes `*i`, `*j` | 61–64, 72, 76, 81–82 |
| Meaning of `i` | last index of the "less than pivot" block: `arr[low..i] < pivot` (doc line 45); set to `low - 1` (line 81), so it is `-1` when no element is smaller | 45, 81 |
| Meaning of `j` | first index of the "greater than pivot" block: `arr[j..high] > pivot` (doc line 47); set to `mid` (line 82) | 47, 82 |
| Between `i` and `j` | `arr[i+1..j-1]` are all equal to the pivot | 46 |
| Namespace | `sorting` (line 41), then an **anonymous** namespace inside it (line 42, closed at 84). It is only visible in the same translation unit, so the test `#include`s the `.cpp` and calls `sorting::partition3` | 41–42, 84 |
| Headers | `<algorithm>` (12), `<cassert>` (13), `<ctime>` (14), `<iostream>` (15), `<vector>` (16). `int32_t` is not given its own `<cstdint>` include; it arrives through the other headers | 12–16 |
| Random/time use | `std::rand()` in `test_int`/`test_double` (142, 145, 163, 166); `std::srand(std::time(nullptr))` in `main` (185). This only affects `main`, never `partition3` | 142–185 |
| Direction | ascending: smaller goes left (line 72), larger goes right (line 76); the tests use `std::is_sorted` with default `<` (154, 178). No descending interface exists | 72, 76, 154 |
| Pivot | `T pivot = (*arr)[high];` | 69 |
| Shortcut | `if (high - low <= 1)`: orders the pair, then `*i = low; *j = high;` | 59–66 |

## Preconditions

- `low <= high`, and both are valid indices into `*arr`. The function reads `(*arr)[high]` at line 60 or 69.
- Empty vector: `high = -1` would read index -1, so it is **unsupported**. The smallest supported collection is 1 element. No rejection code is added.
- `low > high`: unspecified by the source, so it is not tested. (`quicksort` guards it at lines 95 and 120.)

## Hand traces (loop lines 68–82, shortcut lines 59–66)

Loop rule: `<` pivot: swap(a[low++], a[mid++]); `==` pivot: mid++; `>` pivot: swap(a[mid], a[high--]), and mid stays.

### [3, 1, 2], pivot = a[2] = 2

| step | mid | low | high | a | what happens |
|---|---|---|---|---|---|
| 0 | 0 | 0 | 2 | 3 1 2 | start |
| 1 | 0 | 0 | 1 | 2 1 3 | 3 > 2: swap a[0], a[2]; high-- |
| 2 | 1 | 0 | 1 | 2 1 3 | 2 == 2: mid++ |
| 3 | 2 | 1 | 1 | 1 2 3 | 1 < 2: swap a[0], a[1]; low++, mid++ |

mid (2) > high (1), so the loop stops. Result {1, 2, 3}, i = low-1 = 0, j = mid = 2, giving [1] | [2] | [3].

### [2, 2, 1], pivot = 1
1. 2 > 1: swap a[0], a[2], giving {1, 2, 2}, high = 1
2. 1 == 1: mid = 1
3. 2 > 1: swap a[1] with itself, high = 0, and the loop stops

Result {1, 2, 2}, i = -1, j = 1, giving [] | [1] | [2, 2].

### [1, 2, 3], pivot = 3 (already ordered)
1 < 3 and 2 < 3 each swap with themselves, so low = 2 and mid = 2. Then 3 == 3, so mid = 3.
Result {1, 2, 3}, i = 1, j = 3, giving [1, 2] | [3] | [].

### [3, 2, 1], pivot = 1 (reversed)
3 > 1: swap, giving {1, 2, 3}, high = 1. Then 1 == 1: mid = 1. Then 2 > 1: self-swap, high = 0.
Result {1, 2, 3}, i = -1, j = 1.

### [1] (smallest supported, shortcut line 59)
high - low = 0 <= 1, a[0] < a[0] is false, so nothing is swapped. Result {1}, i = 0, j = 0.

### [2, 1] (shortcut)
high - low = 1 <= 1, a[1]=1 < a[0]=2, so they swap. Result {1, 2}, i = 0, j = 1.

These match the issue's expected outputs: [3,1,2] gives [1,2,3] and [2,2,1] gives [1,2,2].

## Comparison with a real run (Step 4)

`work/R22-T037/trace_scratch.cpp` is a scratch copy of the loop with a `printf` after each pass. Its output is in `work/R22-T037/trace.txt`, and every row of the [3, 1, 2] table above matches it.

## Decisive transition

Step 1 of [3, 1, 2] is the decisive one. When `arr[mid] > pivot` (line 75–76), the element is swapped to the right end and `high` shrinks. `mid` does **not** advance, because the element swapped in from `high` has not been examined yet.
