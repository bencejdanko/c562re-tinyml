# tests/target/test_mcu_hardware.py
# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2026 Bence Danko

import re
import sys
import time
import pytest


def test_pigweed_target_suite(target_serial, request):
    """Captures and validates live Pigweed test execution over target UART."""
    timeout_sec = request.config.getoption("--timeout")
    start_time = time.time()
    collected_output = ""

    print(f"\n[TARGET TEST] Listening on {target_serial.port} (Timeout: {timeout_sec}s)...")

    while time.time() - start_time < timeout_sec:
        raw_bytes = target_serial.read(target_serial.in_waiting or 1)
        if raw_bytes:
            chunk = raw_bytes.decode("utf-8", errors="ignore")
            sys.stdout.write(chunk)
            sys.stdout.flush()
            collected_output += chunk

            clean_text = re.sub(r"\x1b\[[0-9;]*m", "", collected_output)

            if re.search(r"\[\s*PASSED\s*\]\s*\d+\s+test", clean_text):
                print("\n[TARGET TEST] All on-target Pigweed test assertions passed on silicon.")
                return

            if re.search(r"\[\s*FAILED\s*\]", clean_text):
                pytest.fail("Pigweed on-target assertion failed on MCU.")

    pytest.fail(f"Target test execution timed out after {timeout_sec} seconds.")
