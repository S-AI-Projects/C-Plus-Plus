#!/usr/bin/env python3
"""Compile fast_integer_input.cpp and execute its documented examples."""

import json
import os
import shlex
import subprocess
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SOURCE = ROOT / "others/fast_integer_input.cpp"
compiler = shlex.split(os.environ.get("CXX", "c++"))

with tempfile.TemporaryDirectory(prefix="r22-t017-") as temporary_directory:
    executable = Path(temporary_directory) / "fast_integer_input"
    compile_command = compiler + [
        "-std=c++17", "-Wall", "-Wextra", "-Wno-register", "-Werror=vla",
        str(SOURCE), "-o", str(executable),
    ]
    compile_result = subprocess.run(compile_command, check=True, capture_output=True, text=True)
    examples = [
        {"input": "0\n", "expected": 0, "trace": [0]},
        {"input": "-2048\n", "expected": -2048, "trace": [2, 20, 204, 2048]},
    ]
    actuals = []
    for example in examples:
        run_result = subprocess.run([str(executable)], input=example["input"], check=True, capture_output=True, text=True)
        actual = int(run_result.stdout.strip())
        assert actual == example["expected"], (example, actual)
        actuals.append({**example, "actual": actual, "stdout": run_result.stdout})

results = {
    "task": "R22-T017",
    "student": "Mohit Sharma",
    "branch": "student/e26b07f0869/R22-T017",
    "contract_source": "others/fast_integer_input.cpp:9-20 documents the accepted input; lines 27-45 parse an optional minus, decimal digits, and apply the sign.",
    "examples": actuals,
    "compile_command": compile_command,
    "compiler_version": subprocess.run(compiler + ["--version"], check=True, capture_output=True, text=True).stdout.splitlines()[0],
    "python_version": subprocess.run(["python3", "--version"], check=True, capture_output=True, text=True).stdout.strip(),
    "compile_exit_code": compile_result.returncode,
    "all_example_exit_codes": [0, 0]
}
(ROOT / "work/R22-T017/results.json").write_text(json.dumps(results, indent=2) + "\n")
print("PASS: documented smallest and signed ordinary fastinput examples")
