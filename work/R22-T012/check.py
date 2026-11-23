#!/usr/bin/env python3
"""Compile the repository hash_search.cpp and verify h boundary values."""

import json
import os
import shlex
import subprocess
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SOURCE = ROOT / "search/hash_search.cpp"
EXPECTED = {0: 0, 10: 0, -1: -1}
compiler = shlex.split(os.environ.get("CXX", "c++"))

with tempfile.TemporaryDirectory(prefix="r22-t012-") as temporary_directory:
    executable = Path(temporary_directory) / "hash_search"
    compile_command = compiler + [
        "-std=c++17", "-Wall", "-Wextra", "-Wno-register", "-Werror=vla",
        str(SOURCE), "-o", str(executable),
    ]
    subprocess.run(compile_command, check=True, capture_output=True, text=True)
    run_command = [str(executable)]
    run_result = subprocess.run(run_command, check=True, capture_output=True, text=True)

actual_cases = []
for line in run_result.stdout.splitlines():
    if line.startswith("H_CASE "):
        _, key_text, expected_text, actual_text = line.split()
        key, expected, actual = int(key_text), int(expected_text), int(actual_text)
        assert expected == actual, (key, expected, actual)
        if key in EXPECTED:
            assert expected == EXPECTED[key], (key, expected, EXPECTED[key])
        actual_cases.append({"key": key, "expected": expected, "actual": actual})
assert len(actual_cases) == 4, actual_cases

results = {
    "task": "R22-T012",
    "student": "Mohit Sharma",
    "branch": "student/e26b07f0869/R22-T012",
    "contract_source": "search/hash_search.cpp:42-48; h(int key) returns key % HASHMAX using C++ signed remainder. The index use is at lines 64-65.",
    "safe_index_boundary": "Nonnegative inputs have indices in [0, HASHMAX); negative outputs are not safe table indices.",
    "cases": actual_cases,
    "compile_command": compile_command,
    "run_command": run_command,
    "compiler_version": subprocess.run(compiler + ["--version"], check=True, capture_output=True, text=True).stdout.splitlines()[0],
    "python_version": subprocess.run(["python3", "--version"], check=True, capture_output=True, text=True).stdout.strip(),
    "run_exit_code": run_result.returncode,
}
(ROOT / "work/R22-T012/results.json").write_text(json.dumps(results, indent=2) + "\n")
print(f"PASS: {len(actual_cases)} hash-function boundary assertions")
