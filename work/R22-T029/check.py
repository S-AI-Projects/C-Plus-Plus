#!/usr/bin/env python3
"""Focused runner for task R22-T029: correctness checks and a timing experiment for Fast_InvSqrt in math/inv_sqrt.cpp.

It builds only the inv_sqrt target with the repository's CMake settings
(C++17, -Wall -Wextra -Wno-register -Werror=vla), runs it, and exits non-zero
if the build fails, an assertion fails, or no checks ran.

Usage (from the repository root):  python3 work/R22-T029/check.py
"""
import pathlib
import re
import shutil
import subprocess
import sys
import tempfile

ROOT = pathlib.Path(__file__).resolve().parents[2]
BUILD = ROOT / "build-R22-T029"
TARGET = "inv_sqrt"
SUBDIR = "math"
TAG = "Fast_InvSqrt"
LOG = pathlib.Path(__file__).resolve().parent / "last_run.txt"


def run(cmd):
    print("$ " + " ".join(cmd), flush=True)
    return subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True)


def main():
    lines = []
    # Debug build keeps assert() active (Release would define NDEBUG).
    configure = ["cmake", "-S", ".", "-B", "build-R22-T029",
                 "-DCMAKE_BUILD_TYPE=Debug", "-DUSE_OPENMP=OFF"]
    build = ["cmake", "--build", "build-R22-T029", "--target", TARGET]
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

    # Timing experiment. -fno-strict-aliasing because Fast_InvSqrt reads the
    # float's bits through reinterpret_cast; without it GCC -O2 warns that
    # this breaks strict aliasing. Timings are recorded, never judged:
    # only correctness decides the exit code.
    here = pathlib.Path(__file__).resolve().parent
    tmp = pathlib.Path(tempfile.mkdtemp())
    try:
        cxx = shutil.which("c++") or shutil.which("g++")
        bench_build = [cxx, "-std=c++17", "-O2", "-fno-strict-aliasing",
                       "-Wall", "-Wextra", "-Wno-return-type",
                       str(here / "bench.cpp"), "-o", str(tmp / "bench")]
        built = run(bench_build)
        lines.append("$ " + " ".join(bench_build[:-3]) + " work/R22-T029/bench.cpp")
        if built.returncode != 0:
            sys.stdout.write(built.stderr)
            print("FAIL: benchmark did not compile")
            return 1
        bench = run([str(tmp / "bench")])
    finally:
        shutil.rmtree(tmp)
    (here / "timings.csv").write_text(bench.stdout)
    sys.stderr.write(bench.stderr)
    lines.append(bench.stderr)
    LOG.write_text("\n".join(lines))
    if bench.returncode != 0:
        print("FAIL: benchmark correctness check failed")
        return 1
    count += 1

    print(f"PASS: {count} checks passed")
    return 0


if __name__ == "__main__":
    sys.exit(main())
