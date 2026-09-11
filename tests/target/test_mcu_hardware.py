# tests/target/test_mcu_hardware.py
# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2026 Bence Danko

import os
import re
import sys
import time
import pytest

ANSI_ESCAPE = re.compile(r"\x1b\[[0-9;]*m")

PASSED_SUMMARY = re.compile(r"^\[\s*PASSED\s*\]\s*\d+\s+test")
FAILED_SUMMARY = re.compile(r"^\[\s*FAILED\s*\]\s*\d+\s+test")
FAILED_LINE = re.compile(r"^\[\s*FAILED\s*\]")

# Pigweed's printing handler emits plain text, so colour is applied here on the host.
# First matching prefix wins.
GREEN, RED, YELLOW, CYAN, BOLD, DIM, RESET = (
    "\x1b[32m", "\x1b[31m", "\x1b[33m", "\x1b[36m", "\x1b[1m", "\x1b[2m", "\x1b[0m"
)
LINE_STYLES = [
    (re.compile(r"^\[\s*(PASSED|OK)\s*\]"), BOLD + GREEN),
    (re.compile(r"^\[\s*FAILED\s*\]"), BOLD + RED),
    (re.compile(r"^\S+:\d+: Failure"), RED),
    (re.compile(r"^\s+(Expected|Actual):"), YELLOW),
    (re.compile(r"^\[\s*DISABLED\s*\]"), YELLOW),
    (re.compile(r"^\[ RUN\s*\]"), CYAN),
    (re.compile(r"^\[[=-]+\]"), DIM),
]


def use_colour() -> bool:
    return "NO_COLOR" not in os.environ and sys.stdout.isatty()


def colourise(line: str) -> str:
    for pattern, style in LINE_STYLES:
        if pattern.search(line):
            return f"{style}{line}{RESET}"
    return line


def test_pigweed_target_suite(target_serial, request):
    """Captures and validates live Pigweed test execution over target UART."""
    timeout_sec = request.config.getoption("--timeout")
    colour = use_colour()
    start_time = time.time()
    pending = ""
    failed = False

    print(f"\n[TARGET TEST] Listening on {target_serial.port} (Timeout: {timeout_sec}s)...")

    while time.time() - start_time < timeout_sec:
        raw_bytes = target_serial.read(target_serial.in_waiting or 1)
        if not raw_bytes:
            continue

        pending += raw_bytes.decode("utf-8", errors="ignore")
        *lines, pending = pending.split("\n")

        for raw_line in lines:
            line = ANSI_ESCAPE.sub("", raw_line).rstrip("\r")
            print(colourise(line) if colour else line, flush=True)

            if FAILED_LINE.search(line):
                failed = True

            # Keep reading after a failing test so Expected/Actual and the summary are shown.
            if failed and FAILED_SUMMARY.search(line):
                pytest.fail("Pigweed on-target assertion failed on MCU.")
            if not failed and PASSED_SUMMARY.search(line):
                print("\n[TARGET TEST] All on-target Pigweed test assertions passed on silicon.")
                return

    if failed:
        pytest.fail("Pigweed on-target assertion failed on MCU (summary not received before timeout).")
    pytest.fail(f"Target test execution timed out after {timeout_sec} seconds.")
