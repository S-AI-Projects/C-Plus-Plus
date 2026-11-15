#!/usr/bin/env python3
"""
check.py — Focused test runner for R22-T001.

Builds circular_queue_using_array_test.cpp with the system C++ compiler,
runs the resulting assertions, and propagates failures (exits non-zero
when any assertion fails).

Usage:
    python3 work/R22-T001/check.py
"""

import json
import os
import platform
import subprocess
import sys
import time
from pathlib import Path

# ── Paths ────────────────────────────────────────────────────────────────
REPO_ROOT = Path(__file__).resolve().parent.parent.parent
SRC_DIR = REPO_ROOT / "operations_on_datastructures"
TEST_SRC = SRC_DIR / "circular_queue_using_array_test.cpp"
WORK_DIR = Path(__file__).resolve().parent
BUILD_DIR = WORK_DIR / "build"
RESULTS_FILE = WORK_DIR / "results.json"
TEST_BINARY = BUILD_DIR / "circular_queue_test"


def find_compiler():
    """Return the first available C++ compiler."""
    for cc in ("g++", "clang++", "c++"):
        try:
            result = subprocess.run(
                [cc, "--version"],
                capture_output=True, text=True, timeout=10
            )
            if result.returncode == 0:
                version_line = result.stdout.strip().split("\n")[0]
                return cc, version_line
        except FileNotFoundError:
            continue
    return None, None


def compile_tests(compiler: str) -> subprocess.CompletedProcess:
    """Compile the test file and return the CompletedProcess."""
    BUILD_DIR.mkdir(parents=True, exist_ok=True)
    cmd = [
        compiler,
        "-std=c++17",
        "-Wall", "-Wextra", "-Wpedantic",
        "-o", str(TEST_BINARY),
        str(TEST_SRC),
    ]
    print(f"[check] Compiling: {' '.join(cmd)}")
    return subprocess.run(cmd, capture_output=True, text=True, timeout=60)


def run_tests() -> subprocess.CompletedProcess:
    """Execute the test binary and return the CompletedProcess."""
    print(f"[check] Running: {TEST_BINARY}")
    return subprocess.run(
        [str(TEST_BINARY)],
        capture_output=True, text=True, timeout=30
    )


def main():
    results = {
        "task": "R22-T001",
        "file_under_test": "operations_on_datastructures/circular_queue_using_array.cpp",
        "function_under_test": "Enque",
        "timestamp": time.strftime("%Y-%m-%dT%H:%M:%S%z"),
        "platform": platform.platform(),
        "python_version": platform.python_version(),
    }

    # ── Find compiler ────────────────────────────────────────────────
    compiler, compiler_version = find_compiler()
    if compiler is None:
        print("[check] ERROR: No C++ compiler found.", file=sys.stderr)
        sys.exit(1)

    results["compiler"] = compiler
    results["compiler_version"] = compiler_version
    print(f"[check] Compiler: {compiler} ({compiler_version})")

    # ── Compile ──────────────────────────────────────────────────────
    comp = compile_tests(compiler)
    results["compile_command"] = comp.args
    results["compile_returncode"] = comp.returncode

    if comp.returncode != 0:
        print("[check] COMPILE FAILED", file=sys.stderr)
        print(comp.stderr, file=sys.stderr)
        results["compile_stderr"] = comp.stderr
        results["outcome"] = "COMPILE_FAILURE"
        RESULTS_FILE.write_text(json.dumps(results, indent=2))
        sys.exit(1)

    print("[check] Compilation succeeded.")

    # ── Run ──────────────────────────────────────────────────────────
    run = run_tests()
    results["test_stdout"] = run.stdout
    results["test_stderr"] = run.stderr
    results["test_returncode"] = run.returncode

    print(run.stdout)

    # Count tests from output
    test_count = run.stdout.count("PASS")
    results["test_count"] = test_count

    if run.returncode != 0:
        print("[check] TESTS FAILED", file=sys.stderr)
        if run.stderr:
            print(run.stderr, file=sys.stderr)
        results["outcome"] = "TEST_FAILURE"
    else:
        if test_count == 0:
            print("[check] WARNING: 0 tests detected.", file=sys.stderr)
            results["outcome"] = "NO_TESTS"
        else:
            print(f"[check] All {test_count} tests passed.")
            results["outcome"] = "PASS"

    # ── Save results ─────────────────────────────────────────────────
    RESULTS_FILE.write_text(json.dumps(results, indent=2))
    print(f"[check] Results saved to {RESULTS_FILE}")

    sys.exit(run.returncode)


if __name__ == "__main__":
    main()
