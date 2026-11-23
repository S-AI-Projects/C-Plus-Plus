#!/usr/bin/env python3
"""Build and execute CombSort and FindNextGap assertions."""

import json
import os
import shlex
import subprocess
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SOURCE = ROOT / "sorting/comb_sort.cpp"
compiler = shlex.split(os.environ.get("CXX", "c++"))

with tempfile.TemporaryDirectory(prefix="r22-t016-") as temporary_directory:
    executable = Path(temporary_directory) / "comb_sort"
    compile_command = compiler + [
        "-std=c++17", "-Wall", "-Wextra", "-Wno-register", "-Werror=vla",
        str(SOURCE), "-o", str(executable),
    ]
    compile_result = subprocess.run(compile_command, check=True, capture_output=True, text=True)
    run_command = [str(executable)]
    run_result = subprocess.run(run_command, input="0\n", check=True, capture_output=True, text=True)
    assert "Comb sort fixture cases passed: 6" in run_result.stdout

actuals = []
for line in run_result.stdout.splitlines():
    if line.startswith("FIXTURE "):
        _, index, input_text, actual_text = line.split(" ", 3)
        actuals.append({"index": int(index), "input": json.loads(input_text.split("=", 1)[1]),
                        "actual": json.loads(actual_text.split("=", 1)[1])})
assert len(actuals) == 6, actuals

results = {
    "task": "R22-T016",
    "student": "Mohit Sharma",
    "branch": "student/e26b07f0869/R22-T016",
    "refactor": "Named the 10/13 shrink calculation shrinkGapByFactor; FindNextGap now names the shrunken_gap before clamping it to at least one.",
    "fixtures": [
        {"input": [3, 1, 2], "expected": [1, 2, 3]},
        {"input": [2, 2, 1], "expected": [1, 2, 2]},
        {"input": [1, 2, 3], "expected": [1, 2, 3]},
        {"input": [3, 2, 1], "expected": [1, 2, 3]},
        {"input": [], "expected": []},
        {"input": [7], "expected": [7]}
    ],
    "fixture_actuals": actuals,
    "gap_cases": [{"input": 13, "expected": 10}, {"input": 2, "expected": 1}, {"input": 1, "expected": 1}, {"input": 0, "expected": 1}],
    "compile_command": compile_command,
    "run_command": run_command + ["(stdin: 0)"],
    "compiler_version": subprocess.run(compiler + ["--version"], check=True, capture_output=True, text=True).stdout.splitlines()[0],
    "python_version": subprocess.run(["python3", "--version"], check=True, capture_output=True, text=True).stdout.strip(),
    "compile_exit_code": compile_result.returncode,
    "run_exit_code": run_result.returncode
}
(ROOT / "work/R22-T016/results.json").write_text(json.dumps(results, indent=2) + "\n")
print("PASS: 6 sort fixtures and 4 FindNextGap boundary assertions")
