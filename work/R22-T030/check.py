#!/usr/bin/env python3
"""Focused runner for task R22-T030: mutation-resistant regression for maxKnapsackValue in dynamic_programming/0_1_knapsack.cpp.

It builds only the 0_1_knapsack target with the repository's CMake settings
(C++17, -Wall -Wextra -Wno-register -Werror=vla), runs it, and exits non-zero
if the build fails, an assertion fails, or no checks ran.

Usage (from the repository root):  python3 work/R22-T030/check.py
"""
import pathlib
import re
import shutil
import subprocess
import sys
import tempfile

ROOT = pathlib.Path(__file__).resolve().parents[2]
BUILD = ROOT / "build-R22-T030"
TARGET = "0_1_knapsack"
SUBDIR = "dynamic_programming"
TAG = "maxKnapsackValue"
LOG = pathlib.Path(__file__).resolve().parent / "last_run.txt"


def run(cmd):
    print("$ " + " ".join(cmd), flush=True)
    return subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True)


def main():
    lines = []
    # Debug build keeps assert() active (Release would define NDEBUG).
    configure = ["cmake", "-S", ".", "-B", "build-R22-T030",
                 "-DCMAKE_BUILD_TYPE=Debug", "-DUSE_OPENMP=OFF"]
    build = ["cmake", "--build", "build-R22-T030", "--target", TARGET]
    for cmd in (configure, build):
        result = run(cmd)
        lines.append("$ " + " ".join(cmd))
        if result.returncode != 0:
            sys.stdout.write(result.stdout + result.stderr)
            print("FAIL: command failed with exit code", result.returncode)
            return result.returncode or 1

    binary = BUILD / SUBDIR / TARGET
    result = run([str(binary)])
    lines.append("$ " + str(binary.relative_to(ROOT)))
    lines.append(result.stderr)
    sys.stderr.write(result.stderr)
    LOG.write_text("\n".join(lines))

    if result.returncode != 0:
        print("FAIL: test binary exited with code", result.returncode)
        return result.returncode if result.returncode > 0 else 1

    match = re.search(r"\[" + re.escape(TAG) + r"\] (\d+) checks passed",
                      result.stderr)
    count = int(match.group(1)) if match else 0
    if count == 0:
        print("FAIL: no checks were reported")
        return 1

    # Mutant: an item that does not fit resets the value instead of keeping
    # the best value of the earlier items. The original passes (above); the
    # mutant must fail the named assertion.
    source = (ROOT / "dynamic_programming" / "0_1_knapsack.cpp").read_text()
    original = "                maxValue[i][j] = maxValue[i - 1][j];\n"
    if source.count(original) != 1:
        print("FAIL: mutation point not found exactly once")
        return 1
    mutant = source.replace(original, "                maxValue[i][j] = 0;\n")
    named = "Assertion `max_value3 == expected_max_value3' failed"
    tmp = pathlib.Path(tempfile.mkdtemp())
    try:
        (tmp / "mutant.cpp").write_text(mutant)
        cxx = shutil.which("c++") or shutil.which("g++")
        built = run([cxx, "-std=c++17", str(tmp / "mutant.cpp"),
                     "-o", str(tmp / "mutant")])
        if built.returncode != 0:
            sys.stdout.write(built.stderr)
            print("FAIL: mutant did not compile")
            return 1
        mutant_run = run([str(tmp / "mutant")])
    finally:
        shutil.rmtree(tmp)
    lines.append("mutant (too-heavy item resets value to 0): exit "
                 + str(mutant_run.returncode))
    lines.append(mutant_run.stderr.strip())
    LOG.write_text("\n".join(lines))
    if mutant_run.returncode == 0 or named not in mutant_run.stderr:
        print("FAIL: the mutant did not fail the named assertion")
        return 1
    print("mutant rejected by:", named)
    count += 1

    print(f"PASS: {count} checks passed")
    return 0


if __name__ == "__main__":
    sys.exit(main())
