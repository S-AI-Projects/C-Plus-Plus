/**
 * @file
 * @brief Executes the worked example of partition3() from quick_sort_3.cpp.
 * @details The real partition3 is tested: quick_sort_3.cpp is included in this
 * translation unit (partition3 lives in an anonymous namespace inside
 * `sorting`, quick_sort_3.cpp lines 41-42), with its main() renamed. No
 * algorithm is duplicated. Expected values were calculated by hand
 * (work/R22-T037/inputs.md).
 */
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <string>
#include <vector>

#define main quick_sort_3_original_main  // keep the file's own main() out
#include "quick_sort_3.cpp"
#undef main

static int checks = 0;

/** Formats a vector as "{a, b, c}". */
static std::string show(const std::vector<int> &v) {
    std::ostringstream os;
    os << "{";
    for (size_t k = 0; k < v.size(); k++) os << (k ? ", " : "") << v[k];
    os << "}";
    return os.str();
}

struct Case {
    std::vector<int> input, expected;
    int32_t i, j;  // expected values of *i and *j
};

/** Runs partition3, prints the displayed line, checks every value. */
static void run_case(const Case &c) {
    std::vector<int> a = c.input;
    int32_t i = -99, j = -99;  // sentinels: must be overwritten
    sorting::partition3(&a, 0, static_cast<int32_t>(a.size()) - 1, &i, &j);
    std::ostringstream line;
    line << "partition3(" << show(c.input) << ") -> " << show(a)
         << ", i=" << i << ", j=" << j;
    std::printf("%s\n", line.str().c_str());
    ++checks;
    if (a != c.expected || i != c.i || j != c.j) {
        std::fprintf(stderr, "FAIL %s: expected %s, i=%d, j=%d\n",
                     line.str().c_str(), show(c.expected).c_str(), c.i, c.j);
        std::exit(1);
    }
}

int main() {
    // issue fixture; loop lines 68-82, ">" branch line 75-76 is decisive
    run_case({{3, 1, 2}, {1, 2, 3}, 0, 2});
    // issue fixture, repeated value; i = low - 1 = -1 (line 81)
    run_case({{2, 2, 1}, {1, 2, 2}, -1, 1});
    // already ordered; "<" branch line 71-72 swaps with itself
    run_case({{1, 2, 3}, {1, 2, 3}, 1, 3});
    // reversed; ">" branch line 75-76, then "==" line 73-74
    run_case({{3, 2, 1}, {1, 2, 3}, -1, 1});
    // smallest supported collection; shortcut lines 59-66
    run_case({{1}, {1}, 0, 0});
    // two elements; shortcut swap line 60-61, i=low, j=high (63-64)
    run_case({{2, 1}, {1, 2}, 0, 1});
    std::printf("CASES_RUN=%d\n", checks);
    return 0;
}
