#!/usr/bin/env python3
# run_target_tests.py
# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2026 Bence Danko

import sys
import time
import re
import serial

PORT = "/dev/ttyACM0"
BAUDRATE = 115200
TIMEOUT_SEC = 6.0

def main():
    print(f"[HOST] Connecting to ST-LINK VCP ({PORT} @ {BAUDRATE} baud)...")
    try:
        ser = serial.Serial(PORT, BAUDRATE, timeout=0.1)
    except Exception as e:
        print(f"[HOST ERROR] Could not open {PORT}: {e}")
        sys.exit(1)

    ser.reset_input_buffer()
    
    print("[HOST] Streaming live on-target test execution from Cortex-M33:\n")
    start_time = time.time()
    collected = ""
    
    while time.time() - start_time < TIMEOUT_SEC:
        data = ser.read(ser.in_waiting or 1)
        if data:
            chunk = data.decode("utf-8", errors="ignore")
            sys.stdout.write(chunk)
            sys.stdout.flush()
            collected += chunk
            
            # Strip ANSI escape codes for reliable assertion matching
            clean_text = re.sub(r'\x1b\[[0-9;]*m', '', collected)
            
            if re.search(r'\[\s*PASSED\s*\]\s*11 test(?:\(s\)|s)\.', clean_text):
                print("\n[HOST INFO] All 11 Pigweed on-target tests passed successfully on STM32C562RE.")
                sys.exit(0)
            elif re.search(r'\[\s*FAILED\s*\]', clean_text):
                print("\n[HOST ERROR] Pigweed on-target hardware test failure detected.")
                sys.exit(1)

    print("\n[HOST ERROR] Timed out waiting for test completion.")
    sys.exit(1)

if __name__ == "__main__":
    main()
