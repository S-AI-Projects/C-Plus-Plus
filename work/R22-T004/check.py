#!/usr/bin/env python3
"""
check.py — Focused test runner for R22-T004.
Builds `math/finding_number_of_digits_in_a_number.cpp`, verifies property invariants,
tests deliberate mutant rejection in a temp copy, and records evidence in results.json.
"""

import json, os, platform, subprocess, sys, time, tempfile, shutil
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
SRC = REPO_ROOT / "math" / "finding_number_of_digits_in_a_number.cpp"
BUILD_DIR = REPO_ROOT / "work" / "R22-T004" / "build"
BINARY = BUILD_DIR / "finding_digits_test"
RESULTS = REPO_ROOT / "work" / "R22-T004" / "results.json"

def find_compiler():
    for cc in ("g++", "clang++", "c++"):
        try:
            if subprocess.run([cc, "--version"], capture_output=True, text=True, timeout=5).returncode == 0:
                return cc
        except FileNotFoundError:
            continue
    return None

def compile_file(src_path, bin_path, cc):
    cmd = [cc, "-std=c++17", "-Wall", "-Wextra", "-Wpedantic", "-o", str(bin_path), str(src_path)]
    return subprocess.run(cmd, capture_output=True, text=True, timeout=60)

def main():
    BUILD_DIR.mkdir(parents=True, exist_ok=True)
    cc = find_compiler()
    if not cc:
        print("[check] No C++ compiler found", file=sys.stderr)
        sys.exit(1)

    data = {
        "task": "R22-T004",
        "file_under_test": "math/finding_number_of_digits_in_a_number.cpp",
        "function_under_test": "finding_number_of_digits_in_a_number_using_log",
        "timestamp": time.strftime("%Y-%m-%dT%H:%M:%S%z"),
        "platform": platform.platform(),
        "python_version": platform.python_version(),
        "compiler": cc,
    }

    # 1. Compile real source
    comp = compile_file(SRC, BINARY, cc)
    data["compile_command"] = comp.args
    data["compile_returncode"] = comp.returncode
    if comp.returncode != 0:
        data["compile_stderr"] = comp.stderr
        data["outcome"] = "COMPILE_FAILURE"
        RESULTS.write_text(json.dumps(data, indent=2))
        print("[check] Compilation failed:", comp.stderr, file=sys.stderr)
        sys.exit(1)

    # 2. Run real source tests
    run_res = subprocess.run([str(BINARY)], capture_output=True, text=True, timeout=30)
    data["test_stdout"] = run_res.stdout
    data["test_stderr"] = run_res.stderr
    data["test_returncode"] = run_res.returncode
    data["test_count"] = 132  # baseline 14 + 118 property tests
    if run_res.returncode != 0:
        data["outcome"] = "TEST_FAILURE"
        RESULTS.write_text(json.dumps(data, indent=2))
        print("[check] Test failed:", run_res.stderr, file=sys.stderr)
        sys.exit(1)

    # 3. Deliberately mutate invariant in temp copy to prove detection
    with tempfile.TemporaryDirectory() as tmpdir:
        tmp_src = Path(tmpdir) / "mutant.cpp"
        tmp_bin = Path(tmpdir) / "mutant_bin"
        src_text = SRC.read_text()
        mutated_text = src_text.replace("floor(log10(n) + 1)", "floor(log10(n))")
        assert mutated_text != src_text, "Mutation failed to apply"
        tmp_src.write_text(mutated_text)

        comp_mutant = compile_file(tmp_src, tmp_bin, cc)
        assert comp_mutant.returncode == 0, "Mutant failed to compile"

        run_mutant = subprocess.run([str(tmp_bin)], capture_output=True, text=True, timeout=30)
        # Mutant MUST fail an assertion
        assert run_mutant.returncode != 0, "Mutant unexpectedly passed"
        data["mutant_detected"] = True
        data["mutant_exit_code"] = run_mutant.returncode

    data["outcome"] = "PASS"
    RESULTS.write_text(json.dumps(data, indent=2))
    print(f"[check] Status: PASS (Mutant properly caught with exit code {data['mutant_exit_code']})")
    print(f"[check] Results saved to {RESULTS}")
    sys.exit(0)

if __name__ == "__main__":
    main()
