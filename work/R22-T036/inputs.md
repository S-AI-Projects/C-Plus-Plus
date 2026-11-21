# Task R22-T036 Notes — math/check_factorial.cpp

## Inputs (Step 2)

**Parameter:** `uint64_t n` (line 28)
- unsigned 64-bit integer; cannot be negative at the type level
- the guard `if (n <= 0)` (line 29) exists but for `uint64_t` only triggers when `n == 0`
  (a "negative" literal like `-24` wraps to a very large uint64_t, which the loop then tests)

**Return type:** `bool` (line 28)

**Exact condition for `true`:** after the loop, `return (n == 1)` (line 48)
- the loop divides `n` in-place by 2, 3, 4, … as long as divisible
- returns true iff all those divisions reduce the value to exactly 1

**Mutation:** yes — the parameter `n` is reassigned inside the loop (`n = n / i;` line 40).
This changes only the local copy on the stack; there are no global variables and no outside side effects. The function is otherwise pure. (Lines 38–42 show the entire state: local int `i`, local uint64_t `n`.)

**Dependencies:**
- `#include <cassert>` (line 12) — used in `tests()`
- `#include <cstdint>` (line 13) — for `uint64_t`, `uint32_t`
- `#include <iostream>` (line 14) — used in `tests()` for `std::cout`
- `namespace math` (lines 20–50) — `is_factorial` lives inside it; callers must use `math::is_factorial`
- No anonymous namespace; the function is reachable from other translation units via the namespace.

**Documented domain (Doxygen + tests):**
- Doxygen says "factorial of some number" with no explicit mention of 0 (lines 22–26)
- The existing `tests()` (line 59) asserts `math::is_factorial(0) == false`
- So 0 is in the documented + tested domain: it returns false.

**Unsupported inputs:** none explicitly forbidden beyond 0 (which is documented as false).
Passing `n > 20!` (i.e., numbers larger than 2432902008176640000) is not tested; behavior is unspecified.

---

## Loop analysis (Step 3 — verify hand calculations)

```
bool is_factorial(uint64_t n) {           // line 28
    if (n <= 0) { return false; }         // line 29  → only 0 triggers for uint64_t
    int i = 2;                            // line 38  — starts at 2, not 1
    while (n % i == 0) {                  // line 39
        n = n / i;                        // line 40
        i++;                              // line 41
    }
    return (n == 1);                      // line 48
}
```

The issue template said "i = 1", but the actual code starts at **i = 2**. Hand-calculations redone:

| Input | Loop trace (n mutated, i starts at 2)                              | Expected |
|-------|---------------------------------------------------------------------|----------|
| 0     | `0 <= 0` → early return                                            | false    |
| 1     | `1 % 2 ≠ 0` → skip loop → `return 1 == 1`                        | **true** |
| 2     | `2 % 2 == 0` → n=1, i=3; `1 % 3 ≠ 0` → `return 1 == 1`         | **true** |
| 5     | `5 % 2 ≠ 0` → skip loop → `return 5 == 1`                        | **false** |
| 6     | `6 % 2 == 0` → n=3, i=3; `3 % 3 == 0` → n=1, i=4; `1 % 4 ≠ 0` → `return 1 == 1` | **true** (3!=6) |
| 120   | `120/2=60, i=3; 60/3=20, i=4; 20/4=5, i=5; 5/5=1, i=6; 1%6≠0` → `return 1==1` | **true** (5!=120) |

Spot-checks: 3×2=6 ✓, 5×4×3×2=120 ✓

**Neighboring input (Step 3 hand calculation):**
`is_factorial(24)`: 24/2=12, 12/3=4, 4/4=1, 1%5≠0 → `return 1==1` → **true** (4!=24)
Verify: 4! = 1×2×3×4 = 24 ✓

---

## Readability improvement (Step 6)

**Before:** `n` (the parameter) is re-used as the running quotient. After `n = n/i` on line 40, the name `n` no longer means "the number being tested" — it means "quotient so far".

**After:** introduce `uint64_t remaining = n;` immediately after the guard, do all loop operations on `remaining`, and return `remaining == 1`. The parameter `n` is never modified.

This is the only change. No formatting pass, no renaming of anything else, no type or logic change.
