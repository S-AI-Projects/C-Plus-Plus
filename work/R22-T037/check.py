#!/usr/bin/env python3
"""Builds/runs the partition3 worked-example test and checks the docs match.

Exits non-zero if the build fails, the test binary fails, no cases ran, or a
displayed output line in the Doxygen comment of partition3 is not reproduced
exactly by the program.
"""
import pathlib
import re
import subprocess
import sys

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parents[1]
SRC = ROOT / "sorting" / "quick_sort_3.cpp"
TEST = ROOT / "sorting" / "quick_sort_3_partition3_test.cpp"
EXE = HERE / "partition3_test.bin"
# Top-level CMakeLists.txt: CMAKE_CXX_STANDARD 17 (line 9) and
# add_compile_options(-Wall -Wextra -Wno-register -Werror=vla) (line 17).
FLAGS = ["-std=c++17", "-Wall", "-Wextra", "-Wno-register", "-Werror=vla"]
CMD = ["g++", *FLAGS, "-o", str(EXE), str(TEST)]


def run(cmd):
    return subprocess.run(cmd, capture_output=True, text=True)


def ver(cmd):
    try:
        return run(cmd).stdout.splitlines()[0]
    except Exception as e:
        return f"unavailable ({e})"


versions = "g++: %s\ncmake: %s\npython: %s\n" % (
    ver(["g++", "--version"]), ver(["cmake", "--version"]),
    sys.version.split()[0])
(HERE / "tool_versions.txt").write_text(versions)
print(versions)
print("Preconditions (source lines, quick_sort_3.cpp before docs were added):")
print("  partition3 signature lines 55-57; shortcut high-low<=1 line 59;")
print("  pivot = arr[high] line 69; i = low-1 line 81; j = mid line 82")
print("Command:", " ".join(CMD))

try:
    b = run(CMD)
    if b.returncode:
        print(b.stdout + b.stderr)
        sys.exit("FAILED: build error")
    r = run([str(EXE)])
finally:
    if EXE.exists():
        EXE.unlink()  # never commit binaries

out = r.stdout + r.stderr
(HERE / "expected_vs_actual.txt").write_text(out)
print(out)
if r.returncode:
    sys.exit(f"FAILED: test binary exited with {r.returncode}")
m = re.search(r"CASES_RUN=(\d+)", r.stdout)
if not m or int(m.group(1)) == 0:
    sys.exit("FAILED: no cases were run")

# The output shown in the source documentation must be what the code prints.
doc = re.findall(r"^\s*\*\s*(partition3\(\{[^)]*\}\) -> .*?)\s*$",
                 SRC.read_text(), re.M)
printed = set(line.rstrip() for line in r.stdout.splitlines())
missing = [d for d in doc if d not in printed]
if not doc or missing:
    sys.exit(f"FAILED: documented output not reproduced: "
             f"{missing or 'none found'}")
print(f"PASSED: {m.group(1)} cases; {len(doc)} documented output lines "
      f"verified")
