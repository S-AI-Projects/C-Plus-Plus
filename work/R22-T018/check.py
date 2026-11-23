#!/usr/bin/env python3
"""Build and run Brent method failure-path and subsequent success checks."""

import json
import os
import shlex
import subprocess
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SOURCE = ROOT / "numerical_methods/brent_method_extrema.cpp"
compiler = shlex.split(os.environ.get("CXX", "c++"))

with tempfile.TemporaryDirectory(prefix="r22-t018-") as temporary_directory:
    executable = Path(temporary_directory) / "brent_method_extrema"
    compile_command = compiler + [
        "-std=c++17", "-Wall", "-Wextra", "-Wno-register", "-Werror=vla",
        str(SOURCE), "-o", str(executable),
    ]
    compile_result = subprocess.run(compile_command, check=True, capture_output=True, text=True)
    run_command = [str(executable)]
    run_result = subprocess.run(run_command, check=True, capture_output=True, text=True)
    marker = "Degenerate interval returned 2 without evaluations"
    assert marker in run_result.stdout, run_result.stdout
    assert "Test 1...." in run_result.stdout and "passed" in run_result.stdout

results = {
    "task": "R22-T018",
    "student": "Mohit Sharma",
    "branch": "student/e26b07f0869/R22-T018",
    "failure_path_source": "numerical_methods/brent_method_extrema.cpp:41-45; endpoint distance <= EPSILON returns lim_a before calling f.",
    "input": {"lim_a": 2.0, "lim_b": 2.0},
    "expected": {"result": 2.0, "objective_evaluations": 0, "subsequent_valid_test": "passes for minimum x=2"},
    "actual": {"result": 2.0, "objective_evaluations": 0, "subsequent_valid_test": "passes"},
    "compile_command": compile_command,
    "run_command": run_command,
    "compiler_version": subprocess.run(compiler + ["--version"], check=True, capture_output=True, text=True).stdout.splitlines()[0],
    "python_version": subprocess.run(["python3", "--version"], check=True, capture_output=True, text=True).stdout.strip(),
    "compile_exit_code": compile_result.returncode,
    "run_exit_code": run_result.returncode
}
(ROOT / "work/R22-T018/results.json").write_text(json.dumps(results, indent=2) + "\n")
print("PASS: degenerate interval fallback and subsequent valid minimization")
