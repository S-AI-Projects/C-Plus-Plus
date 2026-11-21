/**
 * @file cases.cpp
 * @brief Test cases for math::is_factorial — compiled against BEFORE and AFTER
 *        copies by check.py to confirm identical behavior.
 *
 * Build: g++ -std=c++17 -Wall -Wextra -Wno-register -Werror=vla
 *            -DSRC_FILE=\"path/to/check_factorial.cpp\"
 *            -o cases.bin cases.cpp
 *
 * The same source is compiled twice (before and after the refactor) and
 * compared; both must produce identical stdout/stderr.
 */
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#define main original_main
#include SRC_FILE   // set by the runner: -DSRC_FILE="path"
#undef main

static int checks = 0;

#define CHECK_BOOL(input, expected)                                              \
    do {                                                                         \
        ++checks;                                                                \
        bool actual_ = math::is_factorial(static_cast<uint64_t>(input));         \
        if (actual_ != (expected)) {                                             \
            std::fprintf(stderr,                                                 \
                "FAIL math::is_factorial(%llu): expected %d, actual %d\n",       \
                (unsigned long long)(input), (int)(expected), (int)actual_);     \
            std::exit(1);                                                        \
        }                                                                        \
        std::printf("ok   math::is_factorial(%llu): expected %d, actual %d\n",  \
                    (unsigned long long)(input), (int)(expected), (int)actual_); \
    } while (0)

int main() {
    // --- anchor cases from the issue ---
    CHECK_BOOL(1,   true);    // 1! = 1 (and 0! = 1; loop skips since i starts at 2)
    CHECK_BOOL(2,   true);    // 2! = 2
    CHECK_BOOL(5,   false);   // 5 is not a factorial number
    CHECK_BOOL(120, true);    // 5! = 120

    // --- neighboring input, hand-calculated (Step 3) ---
    CHECK_BOOL(24,  true);    // 4! = 24; loop: 24/2=12,12/3=4,4/4=1,1%5≠0 → true

    // --- additional domain fixture ---
    CHECK_BOOL(6,   true);    // 3! = 6
    CHECK_BOOL(0,   false);   // documented in tests() line 59: is_factorial(0)==false

    // --- values from the file's own test suite (cross-check) ---
    CHECK_BOOL(50,       false);  // math/check_factorial.cpp line 57
    CHECK_BOOL(720,      true);   // 6! = 720; math/check_factorial.cpp line 58
    CHECK_BOOL(479001600, true);  // 12! = 479001600; math/check_factorial.cpp line 61

    std::printf("CASES_RUN=%d\n", checks);
    return 0;
}
