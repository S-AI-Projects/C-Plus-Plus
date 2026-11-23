/**
 * @file
 * @brief Timing experiment for Fast_InvSqrt (task R22-T029). Not part of the
 * library: it includes math/inv_sqrt.cpp to time the real function.
 * @details Input size N = number of calls. For each N there are 5
 * repetitions, each with fresh inputs from a fixed seed (log-uniform in
 * [1e-3, 1e3]). Only the loop of Fast_InvSqrt calls is timed; generating
 * inputs and checking results happen outside the timed part. Correctness
 * failures exit non-zero; timings never decide pass or fail.
 */
#define main inv_sqrt_program_main
#include "../../math/inv_sqrt.cpp"
#undef main

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <random>
#include <vector>

int main() {
    const std::vector<int> sizes = {100000, 1000000, 10000000};
    const int reps = 5;
    int failures = 0;
    std::printf("N,rep,ns_total,ns_per_call\n");
    for (int n : sizes) {
        std::vector<double> per_call;
        for (int rep = 0; rep < reps; ++rep) {
            std::mt19937_64 rng(1000003ULL * n + rep);  // fixed seed, fresh inputs
            std::uniform_real_distribution<double> exponent(-3.0, 3.0);
            std::vector<double> x(n), y(n);
            for (int k = 0; k < n; ++k) x[k] = std::pow(10.0, exponent(rng));

            const auto start = std::chrono::steady_clock::now();
            for (int k = 0; k < n; ++k) y[k] = Fast_InvSqrt<double, 2>(x[k]);
            const auto stop = std::chrono::steady_clock::now();

            for (int k = 0; k < n; ++k) {  // correctness, outside the timing
                const double ref = 1.0 / std::sqrt(x[k]);
                if (!(std::fabs(y[k] - ref) <= ref * 5e-6)) ++failures;
            }
            const double ns =
                std::chrono::duration<double, std::nano>(stop - start).count();
            per_call.push_back(ns / n);
            std::printf("%d,%d,%.0f,%.3f\n", n, rep, ns, ns / n);
        }
        std::sort(per_call.begin(), per_call.end());
        std::fprintf(stderr,
                     "[bench] N=%d ns/call median %.3f range %.3f-%.3f\n", n,
                     per_call[reps / 2], per_call.front(), per_call.back());
    }
    std::fprintf(stderr, "[bench] correctness failures: %d\n", failures);
    return failures == 0 ? 0 : 1;
}
