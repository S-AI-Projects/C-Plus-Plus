#!/usr/bin/env python3
"""Build BST assertions and prove the leaf-successor test kills a mutant."""

import json
import os
import shlex
import subprocess
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SOURCE = ROOT / "operations_on_datastructures/inorder_successor_of_bst.cpp"
compiler = shlex.split(os.environ.get("CXX", "c++"))
flags = ["-std=c++17", "-Wall", "-Wextra", "-Wno-register", "-Werror=vla"]

with tempfile.TemporaryDirectory(prefix="r22-t020-") as temporary_directory:
    temporary = Path(temporary_directory)
    executable = temporary / "inorder_successor"
    compile_command = compiler + flags + [str(SOURCE), "-o", str(executable)]
    compile_result = subprocess.run(compile_command, check=True, capture_output=True, text=True)
    run_command = [str(executable)]
    run_result = subprocess.run(run_command, check=True, capture_output=True, text=True)
    marker = "BST fixture passed: root successor=5, leaf successor=2"
    assert marker in run_result.stdout, run_result.stdout

    source = SOURCE.read_text()
    original = "if (current->data < ancestor->data) {"
    mutant = "if (current->data > ancestor->data) {"
    assert original in source
    mutant_source = temporary / "inorder_successor_mutant.cpp"
    mutant_source.write_text(source.replace(original, mutant, 1))
    mutant_executable = temporary / "inorder_successor_mutant"
    mutant_compile = compiler + flags + [str(mutant_source), "-o", str(mutant_executable)]
    subprocess.run(mutant_compile, check=True, capture_output=True, text=True)
    mutant_run = subprocess.run([str(mutant_executable)], capture_output=True, text=True)
    assert mutant_run.returncode != 0, "mutant unexpectedly passed the same assertions"

results = {
    "task": "R22-T020",
    "student": "Mohit Sharma",
    "branch": "student/e26b07f0869/R22-T020",
    "fixture": {"inserted": [4, 2, 6, 1, 3, 5, 7], "inorder": [1, 2, 3, 4, 5, 6, 7]},
    "expected": {"root_4_successor": 5, "leaf_1_successor": 2, "child_links_and_reachability": "valid"},
    "actual": {"root_4_successor": 5, "leaf_1_successor": 2, "child_links_and_reachability": "assertions passed"},
    "mutation": {"changed": "current->data < ancestor->data to current->data > ancestor->data", "exit_code": mutant_run.returncode, "rejected": True},
    "compile_command": compile_command,
    "run_command": run_command,
    "compiler_version": subprocess.run(compiler + ["--version"], check=True, capture_output=True, text=True).stdout.splitlines()[0],
    "python_version": subprocess.run(["python3", "--version"], check=True, capture_output=True, text=True).stdout.strip(),
    "compile_exit_code": compile_result.returncode,
    "run_exit_code": run_result.returncode
}
(ROOT / "work/R22-T020/results.json").write_text(json.dumps(results, indent=2) + "\n")
print("PASS: BST root/leaf successors and tree links; temporary mutant rejected")
