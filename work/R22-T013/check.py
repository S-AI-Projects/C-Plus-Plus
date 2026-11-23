#!/usr/bin/env python3
"""Build and run the quadratic-equation regression and reference assertions."""

import json
import os
import shlex
import subprocess
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SOURCE = ROOT / "math/quadratic_equations_complex_numbers.cpp"
compiler = shlex.split(os.environ.get("CXX", "c++"))

with tempfile.TemporaryDirectory(prefix="r22-t013-") as temporary_directory:
    executable = Path(temporary_directory) / "quadratic_equations"
    compile_command = compiler + [
        "-std=c++17", "-Wall", "-Wextra", "-Wno-register", "-Werror=vla",
        str(SOURCE), "-o", str(executable),
    ]
    compile_result = subprocess.run(compile_command, check=True, capture_output=True, text=True)
    run_command = [str(executable)]
    run_result = subprocess.run(run_command, check=True, capture_output=True, text=True)

reference_line = next(
    (line for line in run_result.stdout.splitlines() if line.startswith("Reference cases passed: ")),
    None,
)
assert reference_line is not None, run_result.stdout
reference_cases = int(reference_line.rsplit(" ", 1)[1])
assert reference_cases == 100, reference_cases

results = {
    "task": "R22-T013",
    "student": "Mohit Sharma",
    "branch": "student/e26b07f0869/R22-T013",
    "fixed_cases": [
        {"input": [1, -2, 1], "expected_roots": [[1, 0], [1, 0]]},
        {"input": [1, 4, 5], "expected_roots": [[-2, -1], [-2, 1]]},
        {"input": [1, -3, 2], "expected_roots": [[1, 0], [2, 0]]},
    ],
    "oracle": "Complex square root of b²-4ac and quadratic formula; independent of the function under test.",
    "reference_cases_passed": reference_cases,
    "compile_command": compile_command,
    "run_command": run_command,
    "compiler_version": subprocess.run(compiler + ["--version"], check=True, capture_output=True, text=True).stdout.splitlines()[0],
    "python_version": subprocess.run(["python3", "--version"], check=True, capture_output=True, text=True).stdout.strip(),
    "compile_exit_code": compile_result.returncode,
    "run_exit_code": run_result.returncode,
}
(ROOT / "work/R22-T013/results.json").write_text(json.dumps(results, indent=2) + "\n")
print(f"PASS: 100 reference-comparison cases and fixed regression assertions")
