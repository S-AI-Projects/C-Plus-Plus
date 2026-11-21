#!/usr/bin/env python3
"""Original must pass; a one-line mutant must fail the same named check."""
import pathlib
import re
import shutil
import subprocess
import sys

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parents[1]
DIR = ROOT / "data_structures" / "cll"
TEST = DIR / "cll_test.cpp"
# Top-level CMakeLists.txt: CMAKE_CXX_STANDARD 17 (line 9),
# add_compile_options(-Wall -Wextra -Wno-register -Werror=vla) (line 17).
FLAGS = ["-std=c++17", "-Wall", "-Wextra", "-Wno-register", "-Werror=vla"]

# `total++;` occurs in insert_front (cll.cpp:50) AND insert_tail (cll.cpp:70).
# This longer pattern only matches the end of insert_tail.
MUTANT_OLD = "        newNode->next = head;\n    }\n    total++;"
MUTANT_NEW = "        newNode->next = head;\n    }\n    total += 2;"
PROTECTED_CHECK = "one-element size"


def run(cmd, **kw):
    return subprocess.run(cmd, capture_output=True, text=True, timeout=30, **kw)


def ver(cmd):
    try:
        return run(cmd).stdout.splitlines()[0]
    except Exception as e:
        return f"unavailable ({e})"


def build_run(workdir, tag):
    exe = HERE / f"{tag}.bin"
    cmd = ["g++", *FLAGS, "-o", str(exe), str(workdir / TEST.name),
           str(workdir / "cll.cpp")]
    b = run(cmd)
    if b.returncode:
        print(b.stdout + b.stderr)
        sys.exit(f"FAILED: {tag} build error")
    try:
        return cmd, run([str(exe)])
    finally:
        exe.unlink(missing_ok=True) if sys.version_info >= (3, 8) else None


versions = "g++: %s\ncmake: %s\npython: %s\n" % (
    ver(["g++", "--version"]), ver(["cmake", "--version"]),
    sys.version.split()[0])
(HERE / "tool_versions.txt").write_text(versions)
print(versions)
print("Preconditions (source lines): constructor sets head=NULL, total=0 at "
      "cll.cpp:9-10; insert_tail increments total at cll.cpp:70; "
      "get_size returns total at cll.cpp:74")

# 1) Original must pass with a nonzero check count.
cmd, r = build_run(DIR, "original")
out = r.stdout + r.stderr
(HERE / "expected_vs_actual.txt").write_text(out)
print("Command:", " ".join(cmd))
print(out)
m = re.search(r"CASES_RUN=(\d+)", r.stdout)
if r.returncode or not m or int(m.group(1)) == 0:
    sys.exit("FAILED: original did not pass")
if f"ok   {PROTECTED_CHECK}:" not in r.stdout:
    sys.exit("FAILED: protected check did not pass on the original")

# 2) Mutant, in a temporary copy only: must fail the SAME named check.
tmp = HERE / "mutant_copy"
shutil.rmtree(tmp, ignore_errors=True)
shutil.copytree(DIR, tmp)
try:
    src = (tmp / "cll.cpp").read_text()  # universal newlines: CRLF -> \n
    if src.count(MUTANT_OLD) != 1:
        sys.exit(f"mutation pattern must occur exactly once, found "
                 f"{src.count(MUTANT_OLD)}: edit MUTANT_OLD in check.py")
    (tmp / "cll.cpp").write_text(src.replace(MUTANT_OLD, MUTANT_NEW, 1))
    _, bad = build_run(tmp, "mutant")
finally:
    shutil.rmtree(tmp, ignore_errors=True)  # the mutant never stays in the repo
bad_out = bad.stdout + bad.stderr
(HERE / "failing_run.txt").write_text(
    f"Mutant: insert_tail `total++;` (cll.cpp:70) -> `total += 2;`\n\n" + bad_out
    + f"\nexit code: {bad.returncode}\n")
if bad.returncode == 0:
    sys.exit("FAILED: mutant passed, so the test does not protect this condition")
if f"FAIL {PROTECTED_CHECK}:" not in bad.stderr:
    sys.exit(f"FAILED: mutant failed, but not on '{PROTECTED_CHECK}':\n{bad_out}")
print(f"Mutant output:\n{bad_out}")
print(f"PASSED: original passes {m.group(1)} checks; mutant fails "
      f"'{PROTECTED_CHECK}' with a wrong value")
