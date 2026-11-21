#!/usr/bin/env python3
"""Builds and runs the repo's dnf_sort.cpp tests plus the scaling experiment.
Only correctness can fail this script; timings are reported, never thresholded."""
import pathlib
import re
import statistics
import subprocess
import sys

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parents[1]
SRC = ROOT / "sorting" / "dnf_sort.cpp"
HARNESS = HERE / "scaling.cpp"
FLAGS = ["-std=c++17", "-Wall", "-Wextra", "-Wno-register", "-Werror=vla"]


def run(cmd, **kw):
    return subprocess.run(cmd, capture_output=True, text=True, **kw)


def ver(cmd):
    try:
        return run(cmd).stdout.splitlines()[0]
    except Exception as e:
        return f"unavailable ({e})"


versions = "g++: %s\ncmake: %s\npython: %s\n" % (
    ver(["g++", "--version"]),
    ver(["cmake", "--version"]),
    sys.version.split()[0],
)
(HERE / "tool_versions.txt").write_text(versions)
print(versions)
print(
    "Preconditions (source lines): dnfSort signature lines 38-39; "
    "supported values lines 48-63; ascending order lines 50-63"
)

# 1) The repository file's own assertions must pass.
own_bin = HERE / "own.bin"
own_cmd = ["g++", *FLAGS, "-o", str(own_bin), str(SRC)]
b = run(own_cmd)
if b.returncode:
    print(b.stdout + b.stderr)
    sys.exit("FAILED: repo file build error")
r = run([str(own_bin)], stdin=subprocess.DEVNULL)
if own_bin.exists():
    own_bin.unlink()
if r.returncode:
    print(r.stdout + r.stderr)
    sys.exit("FAILED: repo file's own tests failed")

# 2) The experiment: correctness gates it, timings are only reported.
scaling_bin = HERE / "scaling.bin"
cmd = [
    "g++",
    *FLAGS,
    f'-DSRC_FILE="{SRC}"',
    "-o",
    str(scaling_bin),
    str(HARNESS),
]
print("Command:", " ".join(cmd))
b = run(cmd)
if b.returncode:
    print(b.stdout + b.stderr)
    sys.exit("FAILED: harness build error")
r = run([str(scaling_bin)])
if scaling_bin.exists():
    scaling_bin.unlink()
out = r.stdout + r.stderr
(HERE / "expected_vs_actual.txt").write_text(out)
if r.returncode:
    print(out)
    sys.exit(f"FAILED: harness exited {r.returncode}")
m = re.search(r"CASES_RUN=(\d+)", r.stdout)
if not m or int(m.group(1)) == 0:
    sys.exit("FAILED: no cases were run")

runs = {}
for n, rep, ns in re.findall(r"^RUN (\d+) (\d+) (\d+)$", r.stdout, re.M):
    runs.setdefault(int(n), []).append(int(ns))
lines = ["size_n,reps,median_ms,min_ms,max_ms"]
meds = {}
for n in sorted(runs):
    v = runs[n]
    meds[n] = statistics.median(v)
    lines.append(
        f"{n},{len(v)},{meds[n]/1e6:.3f},{min(v)/1e6:.3f},{max(v)/1e6:.3f}"
    )
sizes = sorted(meds)
for a, b2 in zip(sizes, sizes[1:]):
    lines.append(
        f"# median({b2}) / median({a}) = {meds[b2]/meds[a]:.2f}  (linear growth predicts about {b2/a:.1f})"
    )
(HERE / "timings.csv").write_text("\n".join(lines) + "\n")
print("\n".join(lines))
print(f"PASSED: {m.group(1)} correctness checks; timings reported, not thresholded")
