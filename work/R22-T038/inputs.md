# Task R22-T038 Notes — backtracking/generate_parentheses.cpp

All line numbers refer to `backtracking/generate_parentheses.cpp` on `master` before modifications.

## Public Interface & Inputs (Step 2)

| Property | Details | Source Line |
|---|---|---|
| Class & Namespace | `class generate_parentheses` inside `namespace backtracking` | lines 22, 26 |
| Public method | `std::vector<std::string> generate(int n)` | lines 33, 70 |
| Inputs | `int n`, representing the number of pairs of parentheses | lines 40, 67, 70 |
| Result | `std::vector<std::string>` returned by value | lines 33, 70, 74 |
| Mutation & State | Member `std::vector<std::string> res` stores combinations (line 28). `generate(int n)` calls `backtracking::generate_parentheses::res.clear();` at line 71 on entry, so results do NOT accumulate on object reuse. `makeStrings` appends to `res` via `res.push_back(str)` at line 56. No global or static variables are mutated. | lines 28, 56, 71 |
| Initial state in `generate` | `str = "(";` with `closed = 0`, `open = 1` | lines 72–73 |
| Dependencies | `<cassert>` (line 14), `<iostream>` (line 15), `<vector>` (line 16), `<string>`. No randomness, time, or external I/O used in the algorithm. | lines 14–16 |

## Recursive Helper Branches (`makeStrings`, lines 45–62)

1. **Closed exceeds open (line 47):**
   `if (closed > open) return;`
   Prunes paths where more closing parentheses have been added than opening ones (invalid prefix).

2. **Terminal unbalanced check (lines 50–53):**
   `if ((str.length() == 2 * n) && (closed != open)) return;`
   Prunes candidate strings of target length $2n$ where open and closed counts do not balance. Returns without appending to `res` (absent-result / pruning path).

3. **Terminal balanced store (lines 55–58):**
   `if (str.length() == 2 * n) { res.push_back(str); return; }`
   Valid combination reached: stores `str` into `res` and returns.

4. **Recursive branch 1 (line 60):**
   `makeStrings(str + ')', n, closed + 1, open);`
   Explores adding a closing parenthesis `')'` first.

5. **Recursive branch 2 (line 61):**
   `makeStrings(str + '(', n, closed, open + 1);`
   Explores adding an opening parenthesis `'('` second.

## Domain & Unsupported Inputs (Step 2 & Step 3)

- **Supported Domain:** $n \ge 1$ (positive number of parenthesis pairs).
- **Unsupported Inputs ($n \le 0$):**
  - In `generate(int n)`, the string begins as `"("` (length 1).
  - For $n = 0$, target length $2n = 0$. Since `str.length()` starts at 1 and only increases, `str.length() == 0` is never satisfied.
  - For $n < 0$, $2n < 0$. In C++, comparing `size_t str.length()` with signed `2 * n` promotes `2 * n` to unsigned `size_t` (e.g. `(size_t)(-2) = 18446744073709551614ULL`), which is never reached.
  - Because `makeStrings` unconditionally recurses on `str + '('` (line 61) without checking `open < n`, calling `generate(n)` with $n \le 0$ causes unbounded recursion leading to stack overflow (SIGSEGV).
  - Precondition: $n \ge 1$. As instructed ("list it as unspecified in your notes, and make sure the PR explains that you tested the actual behavior of the code and added no rejection rule"), $n \le 0$ is documented as unspecified and unsupported; no rejection code is added.

## Exploration Order & Hand Calculations (Step 3)

Because `makeStrings` calls `str + ')'` (line 60) before `str + '('` (line 61), valid combinations are generated with `')'` prioritized over `'('`. This produces reverse-lexicographical order (confirmed by the existing tests in line 92):

| Input $n$ | Catalan $C_n$ | Hand-Calculated Expected Output (Actual Order in Code) | Reached Branches |
|---|---|---|---|
| $n = 1$ | 1 | `{"()"}` | lines 55, 50, 47 |
| $n = 2$ | 2 | `{"()()", "(())"}` | lines 55, 50, 47 |
| $n = 3$ | 5 | `{"()()()", "()(())", "(())()", "(()())", "((()))"}` | lines 55, 50, 47 |
| $n = 4$ | 14 | 14 combinations (anchor test in line 97) | lines 55, 50, 47 |
| $n = 5$ | 42 | 42 combinations | lines 55, 50, 47 |

## Independent Oracle (Step 4)

The oracle iterates over binary masks of length $2n$ from 0 to $2^{2n}-1$. Bit 1 represents `'('` and bit 0 represents `')'`. In counting order from MSB to LSB, valid balanced strings naturally appear in the exact exploration order of `generate(n)`:
- $n = 1$: `{"()"}`
- $n = 2$: `{"()()", "(())"}`
- $n = 3$: `{"()()()", "()(())", "(())()", "(()())", "((()))"}`
The oracle does not use recursion, backtracking, or member state.
