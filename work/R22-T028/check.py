#!/usr/bin/env python3
"""Focused runner for task R22-T028: failure-path checks for HKGraph in graph/hopcroft_karp.cpp.

It builds only the hopcroft_karp target with the repository's CMake settings
(C++17, -Wall -Wextra -Wno-register -Werror=vla), runs it, and exits non-zero
if the build fails, an assertion fails, or no checks ran.

Usage (from the repository root):  python3 work/R22-T028/check.py
"""
import pathlib
import re
import subprocess
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
BUILD = ROOT / "build-R22-T028"
TARGET = "hopcroft_karp"
SUBDIR = "graph"
TAG = "HKGraph"
LOG = pathlib.Path(__file__).resolve().parent / "last_run.txt"


# main() reads a graph from stdin: 1 left, 1 right vertex, no edges.
STDIN = "1 1 0\n"
EXPECTED_STDOUT = "Maximum matching is 0\n"


def run(cmd, stdin=None):
    print("$ " + " ".join(cmd), flush=True)
    return subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True,
                          input=stdin, timeout=120)


def main():
    lines = []
    # Debug build keeps assert() active (Release would define NDEBUG).
    configure = ["cmake", "-S", ".", "-B", "build-R22-T028",
                 "-DCMAKE_BUILD_TYPE=Debug", "-DUSE_OPENMP=OFF"]
    build = ["cmake", "--build", "build-R22-T028", "--target", TARGET]
    for cmd in (configure, build):
        result = run(cmd)
        lines.append("$ " + " ".join(cmd))
        if result.returncode != 0:
            sys.stdout.write(result.stdout + result.stderr)
            print("FAIL: command failed with exit code", result.returncode)
            return result.returncode or 1

    binary = BUILD / SUBDIR / TARGET
    result = run([str(binary)], stdin=STDIN)
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

    if result.stdout != EXPECTED_STDOUT:
        print("FAIL: program printed", repr(result.stdout))
        return 1
    lines.append("stdin " + repr(STDIN) + " -> " + repr(result.stdout))
    LOG.write_text("\n".join(lines))
    count += 1

    print(f"PASS: {count} checks passed")
    return 0


if __name__ == "__main__":
    sys.exit(main())
