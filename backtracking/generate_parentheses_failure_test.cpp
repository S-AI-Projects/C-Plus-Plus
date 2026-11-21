/**
 * @file
 * @brief Regression and oracle comparison tests for backtracking::generate_parentheses.
 * @details Tests the real implementation by including generate_parentheses.cpp with
 *          main() renamed. Verifies hand-calculated anchors, neighboring inputs,
 *          object reuse state isolation, and comparisons against an independent oracle.
 *          Expected values derived in work/R22-T038/inputs.md.
 */
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

#define main generate_parentheses_original_main
#include "generate_parentheses.cpp"
#undef main

static int checks = 0;

/** Formats a vector of strings as {"a", "b", ...} */
static std::string show(const std::vector<std::string> &v) {
    std::string s = "{";
    for (size_t k = 0; k < v.size(); k++) {
        s += (k ? ", \"" : "\"") + v[k] + "\"";
    }
    return s + "}";
}

/** Asserts equality between expected and actual string vectors without assert() */
static void check(const char *label, const std::vector<std::string> &expected,
                  const std::vector<std::string> &actual) {
    ++checks;
    if (expected != actual) {
        std::fprintf(stderr, "FAIL %s: expected %s, actual %s\n", label,
                     show(expected).c_str(), show(actual).c_str());
        std::exit(1);
    }
    std::printf("ok   %s: expected %s, actual %s\n", label,
                show(expected).c_str(), show(actual).c_str());
}

/**
 * Independent oracle: brute-force binary mask iteration over length 2n.
 * Bit 1 represents '(' and bit 0 represents ')'. Counting order from MSB
 * to LSB produces the valid balanced strings in the exact exploration
 * order of generate_parentheses::generate (which visits ')' before '(').
 * Does not use recursion or member state; does not call generate().
 */
static std::vector<std::string> reference_generate(int n) {
    std::vector<std::string> out;
    if (n <= 0) {
        return out;
    }
    const int len = 2 * n;
    const unsigned long total = 1UL << len;
    for (unsigned long mask = 0; mask < total; ++mask) {
        std::string s;
        int depth = 0;
        bool ok = true;
        for (int k = len - 1; k >= 0 && ok; --k) {
            bool is_open = (mask >> k) & 1UL;
            s += is_open ? '(' : ')';
            depth += is_open ? 1 : -1;
            if (depth < 0) {
                ok = false;  // more closed than open at this prefix
            }
        }
        if (ok && depth == 0) {
            out.push_back(s);
        }
    }
    return out;
}

int main() {
    // Smallest supported input n = 1 (lines 55, 50, 47)
    {
        backtracking::generate_parentheses p;
        check("n=1 smallest", {"()"}, p.generate(1));
    }

    // Neighbor input n = 2, hand-calculated in inputs.md (lines 60-61 visit ')' before '(')
    {
        backtracking::generate_parentheses p;
        check("n=2 neighbor", {"()()", "(())"}, p.generate(2));
    }

    // Anchor input n = 3, matches the order in generate_parentheses.cpp line 92
    {
        backtracking::generate_parentheses p;
        check("n=3 anchor",
              {"()()()", "()(())", "(())()", "(()())", "((()))"},
              p.generate(3));
    }

    // Object reuse: verify that calling generate() multiple times on the same object
    // does not accumulate results because res.clear() is called at line 71.
    {
        backtracking::generate_parentheses p;
        auto first = p.generate(1);
        auto second = p.generate(1);
        check("object reuse n=1 first call", {"()"}, first);
        check("object reuse n=1 second call", {"()"}, second);
    }

    // Object reuse with different parameters on the same instance
    {
        backtracking::generate_parentheses p;
        auto r2 = p.generate(2);
        check("object reuse n=2 call", {"()()", "(())"}, r2);
        auto r1 = p.generate(1);
        check("object reuse n=1 call after n=2", {"()"}, r1);
    }

    // Independent oracle comparison across n = 1..5
    for (int n = 1; n <= 5; ++n) {
        backtracking::generate_parentheses p;
        char label[32];
        std::snprintf(label, sizeof label, "oracle comparison n=%d", n);
        check(label, reference_generate(n), p.generate(n));
    }

    // Caller-owned state immutability: earlier returned vector unchanged
    {
        backtracking::generate_parentheses p;
        const auto saved = p.generate(1);
        p.generate(3);
        check("caller-owned result unchanged", {"()"}, saved);
    }

    std::printf("CASES_RUN=%d\n", checks);
    return 0;
}
