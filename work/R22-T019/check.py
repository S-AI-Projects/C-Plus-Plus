#!/usr/bin/env python3
"""Run native quick-sort assertions and a fixed-seed partition scaling study."""

import json
import os
import shlex
import subprocess
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SOURCE = ROOT / "sorting/quick_sort_iterative.cpp"
compiler = shlex.split(os.environ.get("CXX", "c++"))
flags = ["-std=c++17", "-Wall", "-Wextra", "-Wno-register", "-Werror=vla"]

harness = r'''#define main quick_sort_original_main
#include "sorting/quick_sort_iterative.cpp"
#undef main
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <random>
#include <vector>

int main() {
    const std::vector<int> sizes = {128, 512, 2048};
    const unsigned seed = 0x5A17U;
    for (int size : sizes) {
        std::vector<long long> elapsed;
        for (int repetition = 0; repetition < 5; repetition++) {
            std::mt19937 generator(seed + static_cast<unsigned>(size * 17 + repetition));
            std::uniform_int_distribution<int> distribution(-size, size);
            std::vector<int> values(static_cast<size_t>(size));
            for (int &value : values) value = distribution(generator);
            const std::vector<int> original = values;
            const int pivot = values.back();
            const int expected_pivot_index = static_cast<int>(std::count_if(
                original.begin(), original.end() - 1,
                [pivot](int value) { return value <= pivot; }));

            const auto before = std::chrono::steady_clock::now();
            const int pivot_index = sorting::partition(values, 0, size - 1);
            const auto after = std::chrono::steady_clock::now();
            elapsed.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(after - before).count());

            assert(pivot_index == expected_pivot_index);
            assert(values[pivot_index] == pivot);
            for (int index = 0; index < pivot_index; index++) assert(values[index] <= pivot);
            for (int index = pivot_index + 1; index < size; index++) assert(values[index] > pivot);
            std::vector<int> actual_values = values;
            std::vector<int> expected_values = original;
            std::sort(actual_values.begin(), actual_values.end());
            std::sort(expected_values.begin(), expected_values.end());
            assert(actual_values == expected_values);
        }
        std::sort(elapsed.begin(), elapsed.end());
        std::cout << "{\"size\":" << size
                  << ",\"repetitions\":5,\"seed\":" << seed
                  << ",\"median_ns\":" << elapsed[2]
                  << ",\"min_ns\":" << elapsed.front()
                  << ",\"max_ns\":" << elapsed.back() << "}\n";
    }
}
'''

with tempfile.TemporaryDirectory(prefix="r22-t019-") as temporary_directory:
    temporary = Path(temporary_directory)
    native_executable = temporary / "quick_sort_native"
    native_compile = compiler + flags + [str(SOURCE), "-o", str(native_executable)]
    native_build = subprocess.run(native_compile, check=True, capture_output=True, text=True)
    native_run = subprocess.run([str(native_executable)], check=True, capture_output=True, text=True)
    assert "Partition fixture passed" in native_run.stdout

    harness_source = temporary / "partition_benchmark.cpp"
    harness_source.write_text(harness)
    benchmark_executable = temporary / "partition_benchmark"
    benchmark_compile = compiler + flags + ["-I", str(ROOT), str(harness_source), "-o", str(benchmark_executable)]
    benchmark_build = subprocess.run(benchmark_compile, check=True, capture_output=True, text=True)
    benchmark_run = subprocess.run([str(benchmark_executable)], check=True, capture_output=True, text=True)

measurements = [json.loads(line) for line in benchmark_run.stdout.splitlines()]
assert [measurement["size"] for measurement in measurements] == [128, 512, 2048]
assert all(measurement["repetitions"] == 5 and measurement["min_ns"] <= measurement["median_ns"] <= measurement["max_ns"] for measurement in measurements)

results = {
    "task": "R22-T019",
    "student": "Mohit Sharma",
    "branch": "student/e26b07f0869/R22-T019",
    "complexity_claim": "m = end - start + 1; the partition loop compares m-1 elements, therefore one call is Θ(m). Full quicksort worst case remains O(n^2).",
    "measurements": measurements,
    "timing_scope": "Only the partition call; input generation, copying, compilation, process startup, I/O, sorting, and assertions are outside the timed interval.",
    "native_compile_command": native_compile,
    "native_run_command": [str(native_executable)],
    "benchmark_compile_command": benchmark_compile,
    "benchmark_run_command": [str(benchmark_executable)],
    "compiler_version": subprocess.run(compiler + ["--version"], check=True, capture_output=True, text=True).stdout.splitlines()[0],
    "python_version": subprocess.run(["python3", "--version"], check=True, capture_output=True, text=True).stdout.strip(),
    "native_compile_exit_code": native_build.returncode,
    "native_run_exit_code": native_run.returncode,
    "benchmark_compile_exit_code": benchmark_build.returncode,
    "benchmark_run_exit_code": 0
}
(ROOT / "work/R22-T019/results.json").write_text(json.dumps(results, indent=2) + "\n")
print("PASS: partition correctness for 15 fixed-seed runs; median and range recorded for 3 sizes")
