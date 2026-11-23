#!/usr/bin/env python3
"""Focused runner for task R22-T027: executable documentation check for BinaryAddition in greedy_algorithms/binary_addition.cpp.

It builds only the binary_addition target with the repository's CMake settings
(C++17, -Wall -Wextra -Wno-register -Werror=vla), runs it, and exits non-zero
if the build fails, an assertion fails, or no checks ran.

Usage (from the repository root):  python3 work/R22-T027/check.py
"""
import pathlib
import re
import subprocess
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
BUILD = ROOT / "build-R22-T027"
TARGET = "binary_addition"
SUBDIR = "greedy_algorithms"
TAG = "BinaryAddition"
LOG = pathlib.Path(__file__).resolve().parent / "last_run.txt"


def run(cmd):
    print("$ " + " ".join(cmd), flush=True)
    return subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True)


def main():
    lines = []
    # Debug build keeps assert() active (Release would define NDEBUG).
    configure = ["cmake", "-S", ".", "-B", "build-R22-T027",
                 "-DCMAKE_BUILD_TYPE=Debug", "-DUSE_OPENMP=OFF"]
    build = ["cmake", "--build", "build-R22-T027", "--target", TARGET]
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

    # The documented example must be what was executed: read the @code block
    # from the source and compare its "// prints X" with what the binary
    # reported for that example.
    source = (ROOT / "greedy_algorithms" / "binary_addition.cpp").read_text()
    block = re.search(r"@code\n(.*?)@endcode", source, re.S)
    if not block:
        print("FAIL: no @code example in the documentation")
        return 1
    doc = re.search(r'addBinary\("([01]+)", "([01]+)"\);\s*// prints ([01]+)',
                    block.group(1))
    if not doc:
        print("FAIL: @code example has no addBinary call with '// prints'")
        return 1
    a, b, shown = doc.groups()
    call = 'adder.addBinary("%s", "%s");  // prints %s' % (a, b, shown)
    if call not in source.split("@endcode", 1)[1]:
        print("FAIL: the test does not run the documented call:", call)
        return 1
    printed = re.search(r"doc example printed (\S+)", result.stderr)
    if not printed or printed.group(1) != shown:
        print("FAIL: documentation says", shown, "but the example printed",
              printed.group(1) if printed else None)
        return 1
    if int(a, 2) + int(b, 2) != int(shown, 2):
        print("FAIL: documented output is not the sum of the inputs")
        return 1
    lines.append("doc example %s + %s: documented %s, printed %s"
                 % (a, b, shown, printed.group(1)))
    LOG.write_text("\n".join(lines))
    count += 1

    print(f"PASS: {count} checks passed")
    return 0


if __name__ == "__main__":
    sys.exit(main())
