#!/usr/bin/env python3
"""
check.py — Focused test runner for R22-T007.
Builds `dynamic_programming/longest_palindromic_subsequence.cpp`, executes tests and worked examples,
and saves results in `work/R22-T007/results.json`.
"""

import json, os, platform, subprocess, sys, time
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
SRC = REPO_ROOT / "dynamic_programming" / "longest_palindromic_subsequence.cpp"
BUILD_DIR = REPO_ROOT / "work" / "R22-T007" / "build"
BINARY = BUILD_DIR / "longest_palindromic_subsequence"
RESULTS = REPO_ROOT / "work" / "R22-T007" / "results.json"

def find_compiler():
    for cc in ("g++", "clang++", "c++"):
        try:
            if subprocess.run([cc, "--version"], capture_output=True, text=True, timeout=5).returncode == 0:
                return cc
        except FileNotFoundError:
            continue
    return None

def compile_test():
    BUILD_DIR.mkdir(parents=True, exist_ok=True)
    cc = find_compiler()
    if not cc:
        print("[check] No C++ compiler found", file=sys.stderr)
        sys.exit(1)
    cmd = [cc, "-std=c++17", "-Wall", "-Wextra", "-Wpedantic", "-o", str(BINARY), str(SRC)]
    print("[check] Compiling:", " ".join(cmd))
    return subprocess.run(cmd, capture_output=True, text=True, timeout=60)

def main():
    data = {
        "task": "R22-T007",
        "file_under_test": "dynamic_programming/longest_palindromic_subsequence.cpp",
        "function_under_test": "dynamic_programming::lps",
        "timestamp": time.strftime("%Y-%m-%dT%H:%M:%S%z"),
        "platform": platform.platform(),
        "python_version": platform.python_version(),
    }

    cc = find_compiler() or "unknown"
    data["compiler"] = cc

    comp = compile_test()
    data["compile_command"] = comp.args
    data["compile_returncode"] = comp.returncode
    if comp.returncode != 0:
        data["compile_stderr"] = comp.stderr
        data["outcome"] = "COMPILE_FAILURE"
        RESULTS.write_text(json.dumps(data, indent=2))
        print("[check] Compilation failed:", comp.stderr, file=sys.stderr)
        sys.exit(1)

    print("[check] Running:", str(BINARY))
    run_res = subprocess.run([str(BINARY)], capture_output=True, text=True, timeout=30)
    data["test_stdout"] = run_res.stdout
    data["test_stderr"] = run_res.stderr
    data["test_returncode"] = run_res.returncode
    data["test_count"] = 8  # 4 baseline anchors + 2 worked example checks + 2 neighbor assertions
    data["outcome"] = "PASS" if run_res.returncode == 0 else "TEST_FAILURE"

    RESULTS.write_text(json.dumps(data, indent=2))
    print(f"[check] Status: {data['outcome']}, Results saved to {RESULTS}")
    sys.exit(run_res.returncode)

if __name__ == "__main__":
    main()
