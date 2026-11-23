#!/usr/bin/env python3
"""Run wiggle_sort.cpp assertions and prove they reject a temporary mutant."""

import json
import os
import shlex
import subprocess
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SOURCE = ROOT / "sorting/wiggle_sort.cpp"
compiler = shlex.split(os.environ.get("CXX", "c++"))
flags = ["-std=c++17", "-Wall", "-Wextra", "-Wno-register", "-Werror=vla"]

with tempfile.TemporaryDirectory(prefix="r22-t014-") as temporary_directory:
    temporary = Path(temporary_directory)
    executable = temporary / "wiggle_sort"
    compile_command = compiler + flags + [str(SOURCE), "-o", str(executable)]
    compile_result = subprocess.run(compile_command, check=True, capture_output=True, text=True)
    run_command = [str(executable)]
    run_result = subprocess.run(run_command, check=True, capture_output=True, text=True)
    assert "Wiggle cases passed: 106" in run_result.stdout, run_result.stdout

    source = SOURCE.read_text()
    original_condition = "out[i] < out[i + 1]"
    mutant_condition = "out[i] > out[i + 1]"
    assert original_condition in source
    mutant_source = temporary / "wiggle_sort_mutant.cpp"
    mutant_source.write_text(source.replace(original_condition, mutant_condition, 1))
    mutant_executable = temporary / "wiggle_sort_mutant"
    mutant_compile = compiler + flags + [str(mutant_source), "-o", str(mutant_executable)]
    subprocess.run(mutant_compile, check=True, capture_output=True, text=True)
    mutant_run = subprocess.run([str(mutant_executable)], capture_output=True, text=True)
    assert mutant_run.returncode != 0, "mutated comparison unexpectedly passed"

fixture_actuals = []
for line in run_result.stdout.splitlines():
    if line.startswith("FIXTURE "):
        _, index, input_text, output_text = line.split(" ", 3)
        fixture_actuals.append({"index": int(index), "input": json.loads(input_text.split("=", 1)[1]),
                                "actual": json.loads(output_text.split("=", 1)[1])})
assert len(fixture_actuals) == 6, fixture_actuals

results = {
    "task": "R22-T014",
    "student": "Mohit Sharma",
    "branch": "student/e26b07f0869/R22-T014",
    "contract_source": "sorting/wiggle_sort.cpp:42-76; documents the peak-at-even-indices wave pattern and implements a copied return value without input mutation.",
    "fixtures": [
        {"input": [3, 1, 2], "expected": [3, 1, 2]},
        {"input": [2, 2, 1], "expected": [2, 1, 2]},
        {"input": [1, 2, 3], "expected": [2, 1, 3]},
        {"input": [3, 2, 1], "expected": [3, 1, 2]},
        {"input": [], "expected": []},
        {"input": [7], "expected": [7]}
    ],
    "fixture_actuals": fixture_actuals,
    "fixed_seed_generated_cases": 100,
    "native_cases_passed": 106,
    "mutant": {"condition_changed": "out[i] < out[i + 1] → out[i] > out[i + 1]", "exit_code": mutant_run.returncode, "rejected": True},
    "compile_command": compile_command,
    "run_command": run_command,
    "compiler_version": subprocess.run(compiler + ["--version"], check=True, capture_output=True, text=True).stdout.splitlines()[0],
    "python_version": subprocess.run(["python3", "--version"], check=True, capture_output=True, text=True).stdout.strip(),
    "compile_exit_code": compile_result.returncode,
    "run_exit_code": run_result.returncode,
}
(ROOT / "work/R22-T014/results.json").write_text(json.dumps(results, indent=2) + "\n")
print("PASS: 106 wiggle/multiplicity/nonmutation cases; temporary mutant rejected")
