/**
 * @file scaling.cpp
 * @brief Scaling experiment and regression harness for sorting::dnf_sort::dnfSort.
 *        Only the dnfSort call is timed; correctness assertions run outside the timer.
 *        Derived in work/R22-T039/inputs.md.
 */
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <string>
#include <vector>

#define main dnf_sort_original_main
#include SRC_FILE  // set by check.py: -DSRC_FILE="path/to/dnf_sort.cpp"
#undef main

/** Adapts to the real signature and namespace of dnfSort */
static void run_dnf(std::vector<int> &v) {
    v = sorting::dnf_sort::dnfSort(v);
}

static int checks = 0;

static std::string show(const std::vector<int> &v) {
    std::string s = "[";
    for (size_t k = 0; k < v.size(); k++) {
        s += (k ? ", " : "") + std::to_string(v[k]);
    }
    return s + "]";
}

static void expect_sorted(const char *label, std::vector<int> in,
                          const std::vector<int> &expected) {
    std::vector<int> original = in;
    run_dnf(in);
    ++checks;
    if (in != expected) {
        std::fprintf(stderr, "FAIL %s: input %s, expected %s, actual %s\n",
                     label, show(original).c_str(), show(expected).c_str(),
                     show(in).c_str());
        std::exit(1);
    }
    std::printf("ok   %s: input %s, expected %s, actual %s\n", label,
                show(original).c_str(), show(expected).c_str(),
                show(in).c_str());
}

int main() {
    // Small fixture with literal, hand-calculated expectations (domain {0, 1, 2})
    expect_sorted("fixture 1", {2, 0, 1}, {0, 1, 2});
    expect_sorted("fixture 2 repeated", {2, 2, 1}, {1, 2, 2});
    expect_sorted("already ordered", {0, 1, 2}, {0, 1, 2});
    expect_sorted("reversed", {2, 1, 0}, {0, 1, 2});
    expect_sorted("smallest", {0}, {0});

    // Scaling experiment: three sizes doubling each time, 5 repetitions each, fixed seeds.
    const int sizes[] = {1000000, 2000000, 4000000};
    for (int n : sizes) {
        for (int rep = 0; rep < 5; rep++) {
            std::mt19937 rng(12345u + rep);  // fixed seed
            std::vector<int> v(n);
            for (int &x : v) {
                x = static_cast<int>(rng() % 3);  // values 0, 1, 2 only
            }
            std::vector<int> expected = v;
            std::sort(expected.begin(), expected.end());  // oracle, outside timer

            auto t0 = std::chrono::steady_clock::now();
            run_dnf(v);  // the only timed work
            auto t1 = std::chrono::steady_clock::now();

            ++checks;  // correctness check outside timer
            if (v != expected) {
                std::fprintf(stderr, "FAIL scaling n=%d rep=%d: output not sorted\n", n, rep);
                std::exit(1);
            }
            long long ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
            std::printf("RUN %d %d %lld\n", n, rep, ns);
        }
    }
    std::printf("CASES_RUN=%d\n", checks);
    return 0;
}
