#!/usr/bin/env python3
"""
Runs identical cases against the BEFORE copy and the AFTER refactored file.
Exits non-zero if a build fails, a case fails, the case count is 0, or
the before/after outputs differ.
"""
import pathlib
import re
import subprocess
import sys

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parents[1]
BEFORE = HERE / "check_factorial_before.cpp"
AFTER  = ROOT / "math" / "check_factorial.cpp"
CASES  = HERE / "cases.cpp"
FLAGS  = ["-std=c++17", "-Wall", "-Wextra", "-Wno-register", "-Werror=vla"]


def run(cmd):
    return subprocess.run(cmd, capture_output=True, text=True)


def ver(cmd):
    try:
        return run(cmd).stdout.splitlines()[0]
    except Exception as e:
        return f"unavailable ({e})"


def build_run(src, tag):
    exe = HERE / f"{tag}.bin"
    cmd = ["g++", *FLAGS, f'-DSRC_FILE="{src}"', "-o", str(exe), str(CASES)]
    b = run(cmd)
    if b.returncode:
        print(b.stdout + b.stderr)
        sys.exit(f"FAILED: {tag} build error")
    r = run([str(exe)])
    out = r.stdout + r.stderr
    (HERE / f"{tag}_output.txt").write_text(out)
    m = re.search(r"CASES_RUN=(\d+)", r.stdout)
    if r.returncode or not m or int(m.group(1)) == 0:
        print(out)
        sys.exit(f"FAILED: {tag} run (exit {r.returncode})")
    exe.unlink()   # never commit binaries
    return cmd, out, int(m.group(1))


# --- tool versions ---
versions = "g++: %s\ncmake: %s\npython: %s\n" % (
    ver(["g++", "--version"]),
    ver(["cmake", "--version"]),
    sys.version.split()[0],
)
(HERE / "tool_versions.txt").write_text(versions)
print(versions)

# --- source line references ---
print("Preconditions (source lines):")
print("  math::is_factorial signature: math/check_factorial.cpp line 28")
print("  guard (n<=0 → false):         math/check_factorial.cpp line 29")
print("  loop (while remaining % i):   math/check_factorial.cpp line 39")
print("  domain test (0 → false):      math/check_factorial.cpp line 59 (tests())")
print()

# --- build and run both copies ---
cmd_b, out_b, n_b = build_run(BEFORE, "before")
cmd_a, out_a, n_a = build_run(AFTER,  "after")

(HERE / "expected_vs_actual.txt").write_text(
    "BEFORE:\n" + out_b + "\nAFTER:\n" + out_a
)

print("AFTER output:")
print(out_a)

if out_b != out_a:
    sys.exit("FAILED: before and after outputs differ")

print("Command:", " ".join(cmd_a))
print(f"PASSED: {n_a} cases run, before and after outputs are identical")
