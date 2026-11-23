#!/usr/bin/env python3
"""Focused runner for task R22-T026: before/after checks for the forward-elimination refactor in numerical_methods/gaussian_elimination.cpp.

It builds only the gaussian_elimination target with the repository's CMake settings
(C++17, -Wall -Wextra -Wno-register -Werror=vla), runs it, and exits non-zero
if the build fails, an assertion fails, or no checks ran.

Usage (from the repository root):  python3 work/R22-T026/check.py
"""
import pathlib
import re
import shutil
import subprocess
import sys
import tempfile

ROOT = pathlib.Path(__file__).resolve().parents[2]
BUILD = ROOT / "build-R22-T026"
TARGET = "gaussian_elimination"
SUBDIR = "numerical_methods"
TAG = "gaussian_elimination"
LOG = pathlib.Path(__file__).resolve().parent / "last_run.txt"


# The file before the refactor, pinned to the upstream commit the fork is based on.
ORIGINAL_REV = "fdcc29b0ac2848e9d1a8d926e171cc5b39f97759"
SOURCE = "numerical_methods/gaussian_elimination.cpp"

HEADER = ("Matrix size: \nEnter value of the matrix: \n\n"
          "Matrix using Gaussian Elimination method: \n")
MIDDLE = "\nValue of the Gaussian Elimination method: \n"
# (stdin, hand-calculated stdout)
CASES = [
    # smallest valid input: 2x = 6
    ("1\n2 6\n", HEADER + "2 6 \n" + MIDDLE + "x0= 3\n"),
    # 2x + y = 5, x - y = 1
    ("2\n2 1 5\n1 -1 1\n",
     HEADER + "2 1 5 \n0 -1.5 -1.5 \n" + MIDDLE + "x1= 1\nx0= 2\n"),
    # x + y + z = 6, 2y + 5z = -4, 2x + 5y - z = 27
    ("3\n1 1 1 6\n0 2 5 -4\n2 5 -1 27\n",
     HEADER + "1 1 1 6 \n0 2 5 -4 \n0 0 -10.5 21 \n" + MIDDLE
     + "x2= -2\nx1= 3\nx0= 5\n"),
]


def run(cmd, stdin=None):
    print("$ " + " ".join(cmd), flush=True)
    return subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True,
                          input=stdin, timeout=120)


def main():
    lines = []
    # Debug build keeps assert() active (Release would define NDEBUG).
    configure = ["cmake", "-S", ".", "-B", "build-R22-T026",
                 "-DCMAKE_BUILD_TYPE=Debug", "-DUSE_OPENMP=OFF"]
    build = ["cmake", "--build", "build-R22-T026", "--target", TARGET]
    for cmd in (configure, build):
        result = run(cmd)
        lines.append("$ " + " ".join(cmd))
        if result.returncode != 0:
            sys.stdout.write(result.stdout + result.stderr)
            print("FAIL: command failed with exit code", result.returncode)
            return result.returncode or 1

    binary = BUILD / SUBDIR / TARGET
    result = run([str(binary)], stdin=CASES[0][0])
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

    # Build the original file from git so before and after can be compared.
    shown = run(["git", "show", ORIGINAL_REV + ":" + SOURCE])
    if shown.returncode != 0:
        sys.stdout.write(shown.stderr)
        print("FAIL: could not read the original file from git")
        return 1
    tmp = pathlib.Path(tempfile.mkdtemp())
    try:
        (tmp / "original.cpp").write_text(shown.stdout)
        cxx = shutil.which("c++") or shutil.which("g++")
        built = run([cxx, "-std=c++17", str(tmp / "original.cpp"),
                     "-o", str(tmp / "original")])
        if built.returncode != 0:
            sys.stdout.write(built.stderr)
            print("FAIL: original did not compile")
            return 1
        for stdin, expected in CASES:
            after = run([str(binary)], stdin=stdin).stdout
            before = run([str(tmp / "original")], stdin=stdin).stdout
            lines.append("stdin " + repr(stdin))
            lines.append("  expected " + repr(expected))
            lines.append("  after    " + repr(after))
            lines.append("  before   " + repr(before))
            if after != expected or before != expected:
                print("FAIL: output differs for stdin", repr(stdin))
                print("expected", repr(expected))
                print("after   ", repr(after))
                print("before  ", repr(before))
                LOG.write_text("\n".join(lines))
                return 1
            count += 2
    finally:
        shutil.rmtree(tmp)
    LOG.write_text("\n".join(lines))

    print(f"PASS: {count} checks passed")
    return 0


if __name__ == "__main__":
    sys.exit(main())
