#!/usr/bin/env python3
"""
Test runner for backtracking::generate_parentheses regression and oracle tests.
Verifies compilation, execution, case count, and tool versions.
"""
import pathlib
import re
import subprocess
import sys

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parents[1]
SRC = ROOT / "backtracking" / "generate_parentheses.cpp"
TEST = ROOT / "backtracking" / "generate_parentheses_failure_test.cpp"
EXE = HERE / "generate_parentheses_test.bin"

# Flags confirmed from top-level CMakeLists.txt (CMAKE_CXX_STANDARD 17, line 9; add_compile_options line 17)
FLAGS = ["-std=c++17", "-Wall", "-Wextra", "-Wno-register", "-Werror=vla"]
CMD = ["g++", *FLAGS, "-o", str(EXE), str(TEST)]


def run(cmd):
    return subprocess.run(cmd, capture_output=True, text=True)


def ver(cmd):
    try:
        return run(cmd).stdout.splitlines()[0]
    except Exception as e:
        return f"unavailable ({e})"


# Tool versions
versions = "g++: %s\ncmake: %s\npython: %s\n" % (
    ver(["g++", "--version"]),
    ver(["cmake", "--version"]),
    sys.version.split()[0],
)
(HERE / "tool_versions.txt").write_text(versions)
print(versions)

# Preconditions and branch references
print("Preconditions and source lines (backtracking/generate_parentheses.cpp):")
print("  class generate_parentheses: line 26")
print("  generate(n) entry & res.clear(): lines 70-71")
print("  makeStrings pruning (closed > open): line 47")
print("  makeStrings pruning (str.length() == 2*n && closed != open): line 50")
print("  makeStrings base case (str.length() == 2*n): line 55")
print("  makeStrings recursion ')' before '(': lines 60-61")
print("  supported domain: n >= 1 (n <= 0 unspecified, causes unbounded recursion)")
print("Command:", " ".join(CMD))

try:
    b = run(CMD)
    if b.returncode != 0:
        print(b.stdout + b.stderr)
        sys.exit("FAILED: build error")

    r = run([str(EXE)])
finally:
    if EXE.exists():
        EXE.unlink()  # never commit binaries

out = r.stdout + r.stderr
(HERE / "expected_vs_actual.txt").write_text(out)
print(out)

if r.returncode != 0:
    sys.exit(f"FAILED: test binary exited with {r.returncode}")

m = re.search(r"CASES_RUN=(\d+)", r.stdout)
if not m or int(m.group(1)) == 0:
    sys.exit("FAILED: no cases were run")

print(f"PASSED: {m.group(1)} cases verified successfully")
