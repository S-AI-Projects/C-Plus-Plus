#!/usr/bin/env python3
"""Build and run the palindrome_partitioning.cpp assertions."""

import json
import os
import shlex
import subprocess
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SOURCE = ROOT / "dynamic_programming/palindrome_partitioning.cpp"
EXPECTED = {"": 0, "a": 0, "aba": 0, "ab": 1, "abba": 0,
            "nitik": 2, "ababbbabbababa": 3, "abdc": 3}
compiler = shlex.split(os.environ.get("CXX", "c++"))

with tempfile.TemporaryDirectory(prefix="r22-t011-") as temporary_directory:
    executable = Path(temporary_directory) / "palindrome_partitioning"
    compile_command = compiler + [
        "-std=c++17", "-Wall", "-Wextra", "-Wno-register", "-Werror=vla",
        str(SOURCE), "-o", str(executable),
    ]
    compile_result = subprocess.run(compile_command, check=True, capture_output=True, text=True)
    run_command = [str(executable)]
    run_result = subprocess.run(run_command, check=True, capture_output=True, text=True)

actual_cases = [json.loads(line) for line in run_result.stdout.splitlines()]
assert len(actual_cases) == len(EXPECTED), (len(EXPECTED), len(actual_cases))
for case in actual_cases:
    expected = EXPECTED[case["input"]]
    assert case["expected"] == expected, case
    assert case["actual"] == expected, case

results = {
    "task": "R22-T011",
    "student": "Mohit Sharma",
    "branch": "student/e26b07f0869/R22-T011",
    "contract": "String is compared verbatim and case-sensitively; no normalization; empty input has zero cuts.",
    "cases": actual_cases,
    "compile_command": compile_command,
    "run_command": run_command,
    "compiler_version": subprocess.run(compiler + ["--version"], check=True, capture_output=True, text=True).stdout.splitlines()[0],
    "python_version": subprocess.run(["python3", "--version"], check=True, capture_output=True, text=True).stdout.strip(),
    "compile_exit_code": compile_result.returncode,
    "run_exit_code": run_result.returncode,
    "stderr": run_result.stderr,
}
(ROOT / "work/R22-T011/results.json").write_text(json.dumps(results, indent=2) + "\n")
print(f"PASS: {len(actual_cases)} explicit palindrome-partitioning cases")
