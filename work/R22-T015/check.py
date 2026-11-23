#!/usr/bin/env python3
"""Compile and execute the doubly linked list instance-isolation assertions."""

import json
import os
import shlex
import subprocess
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SOURCE = ROOT / "data_structures/doubly_linked_list.cpp"
compiler = shlex.split(os.environ.get("CXX", "c++"))

with tempfile.TemporaryDirectory(prefix="r22-t015-") as temporary_directory:
    executable = Path(temporary_directory) / "doubly_linked_list"
    compile_command = compiler + [
        "-std=c++17", "-Wall", "-Wextra", "-Wno-register", "-Werror=vla",
        str(SOURCE), "-o", str(executable),
    ]
    compile_result = subprocess.run(compile_command, check=True, capture_output=True, text=True)
    run_command = [str(executable)]
    run_result = subprocess.run(run_command, input="0\n", check=True, capture_output=True, text=True)
    assert "Native checks passed: 3-node links, singleton, isolated repeat" in run_result.stdout

results = {
    "task": "R22-T015",
    "student": "Mohit Sharma",
    "branch": "student/e26b07f0869/R22-T015",
    "external_state": "The old file-scope start pointer was moved into each double_linked_list instance.",
    "expected": {
        "three_node_values": [10, 20, 30],
        "three_node_prev_next_links": "consistent in both directions",
        "singleton": {"value": 7, "prev": None, "next": None},
        "second_instance_isolated": True,
        "fresh_repeat_matches": True
    },
    "actual": "All native assertions passed; runner output confirms 3-node links, singleton, and isolated repeat.",
    "compile_command": compile_command,
    "run_command": run_command + ["(stdin: 0)"],
    "compiler_version": subprocess.run(compiler + ["--version"], check=True, capture_output=True, text=True).stdout.splitlines()[0],
    "python_version": subprocess.run(["python3", "--version"], check=True, capture_output=True, text=True).stdout.strip(),
    "compile_exit_code": compile_result.returncode,
    "run_exit_code": run_result.returncode
}
(ROOT / "work/R22-T015/results.json").write_text(json.dumps(results, indent=2) + "\n")
print("PASS: node values, links, singleton and repeated-instance isolation")
