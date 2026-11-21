# Task R22-T039 Notes — sorting/dnf_sort.cpp

All line numbers refer to `sorting/dnf_sort.cpp` on `master` before modifications.

## Public Interface & Inputs (Step 2)

| Property | Details | Source Line |
|---|---|---|
| Function Signature | `template <typename T> std::vector<T> dnfSort(const std::vector<T> &in_arr)` | line 38–39 |
| Namespaces | `namespace sorting` -> `namespace dnf_sort` (qualified: `sorting::dnf_sort::dnfSort`) | lines 23, 30 |
| Parameters | `const std::vector<T> &in_arr` (passed as const reference) | line 39 |
| Return value | `std::vector<T>` by value (returns a sorted copy `arr`) | lines 39, 65 |
| Mutation | Does NOT mutate `in_arr` in place (it is `const`). Creates a local copy `std::vector<T> arr(in_arr);` (line 40), mutates `arr` via `std::swap` (lines 51, 61), and returns `arr` (line 65). No global or static variables are mutated. | lines 40, 51, 61, 65 |
| Sort Direction | Ascending: values equal to 0 are swapped to `lo++` (front), values equal to 1 are left in middle (`mid++`), values equal to 2 are swapped to `hi--` (back). Result is ordered `0 <= 1 <= 2`. | lines 50–63 |
| Supported Values | Domain: $\{0, 1, 2\}$ only. The switch statement (lines 48–63) handles only `case 0:`, `case 1:`, and `case 2:`. There is no `default:` case, so any element $\notin \{0, 1, 2\}$ causes neither pointer to advance, producing an infinite loop (`mid <= hi`). Header comment confirms: "C++ program to sort an array with 0, 1 and 2 in a single pass". | lines 7, 48–63 |
| Dependencies | `<algorithm>` (line 13, for `std::is_sorted`), `<cassert>` (line 14, for `assert`), `<cstdint>` (line 15, for `uint64_t`), `<iostream>` (line 16, for `std::cout` and `std::swap`), `<vector>` (line 17, for `std::vector`). | lines 13–17 |
| Preconditions | 1. Input vector contains elements strictly in $\{0, 1, 2\}$.<br>2. $n \ge 1$: for $n = 0$, `arr.size() - 1` evaluates to `UINT64_MAX` under `uint64_t hi`, so `mid <= hi` (`0 <= UINT64_MAX`) succeeds and accesses `arr[0]` out of bounds. An empty collection is unspecified and unsupported; smallest supported size is 1 element ($n = 1$). | lines 41–47 |
| Existing Complexity Comment | File details comment (lines 8–9): "Since one traversal of the array is there hence it works in O(n) time complexity." Doxygen comment on `dnfSort` (lines 31–37) was outdated and did not mention complexity. | lines 8–9, 31–37 |

## Complexity Reasoning (Step 3)

Let $n$ be the number of elements in the array (`arr.size()`).
The algorithm maintains three pointers:
- `lo = 0` (boundary of 0s)
- `mid = 0` (current unexamined element)
- `hi = n - 1` (boundary of 2s)

The loop condition is `while (mid <= hi)`.
Consider the quantity $W = \text{hi} - \text{mid} + 1$, representing the number of unplaced elements.
Initially, $W = (n - 1) - 0 + 1 = n$.
In every iteration of the `while` loop:
- `case 0`: `lo++`, `mid++` $\implies \text{mid}$ increases by 1, so $W$ decreases by 1.
- `case 1`: `mid++` $\implies \text{mid}$ increases by 1, so $W$ decreases by 1.
- `case 2`: `hi--` $\implies \text{hi}$ decreases by 1, so $W$ decreases by 1.

Because exactly one of the three cases is executed per iteration (for inputs in $\{0, 1, 2\}$), $W$ decreases by exactly 1 in every iteration. The loop terminates when $\text{mid} > \text{hi}$, i.e., $W = 0$.
Therefore, for ANY input array of length $n$ consisting of $\{0, 1, 2\}$, the loop executes **exactly $n$ iterations**.
- Best-case time complexity: $\mathcal{O}(n)$
- Worst-case time complexity: $\mathcal{O}(n)$
- Best case equals worst case.
- Auxiliary space: $\mathcal{O}(1)$ beyond the returned copy.

*Note on empirical observations:* The scaling experiment measures wall-clock execution time across input sizes $n \in \{1\,000\,000, 2\,000\,000, 4\,000\,000\}$. The empirical timings show near-linear scaling agreeing with this derivation, but represent empirical observations rather than a formal proof.

## Hand-Calculated Small Outputs (Step 4)

| Case | Input | Expected Output | Notes |
|---|---|---|---|
| Fixture 1 | `[2, 0, 1]` | `[0, 1, 2]` | 3 distinct values, full permutation |
| Fixture 2 (repeated) | `[2, 2, 1]` | `[1, 2, 2]` | Repeated elements, absent 0 |
| Already ordered | `[0, 1, 2]` | `[0, 1, 2]` | No swaps required |
| Reversed | `[2, 1, 0]` | `[0, 1, 2]` | Full reversal |
| Smallest supported | `[0]` | `[0]` | Single-element boundary input ($n = 1$) |
