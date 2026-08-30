# tests/target/conftest.py
# SPDX-License-Identifier: Apache-2.0
# Copyright (c) 2026 Bence Danko

import glob
import pytest
import serial


def find_default_stlink_port() -> str:
    """Auto-detect ST-Link Virtual COM Port on Linux."""
    ports = glob.glob("/dev/serial/by-id/*STMicroelectronics*")
    if ports:
        return ports[0]
    acm_ports = glob.glob("/dev/ttyACM*")
    if acm_ports:
        return acm_ports[0]
    return "/dev/ttyACM0"


def pytest_addoption(parser):
    parser.addoption(
        "--port",
        action="store",
        default=find_default_stlink_port(),
        help="Serial port for target hardware (e.g. /dev/ttyACM0)",
    )
    parser.addoption(
        "--baudrate",
        action="store",
        type=int,
        default=115200,
        help="Baudrate for target UART stream",
    )
    parser.addoption(
        "--timeout",
        action="store",
        type=float,
        default=8.0,
        help="Timeout in seconds for test execution",
    )


@pytest.fixture(scope="session")
def target_serial(request):
    """Session fixture managing the target MCU UART connection."""
    port = request.config.getoption("--port")
    baudrate = request.config.getoption("--baudrate")

    try:
        ser = serial.Serial(port, baudrate, timeout=0.1)
    except serial.SerialException as exc:
        pytest.fail(f"Could not connect to target device at {port}: {exc}")

    ser.reset_input_buffer()
    yield ser
    ser.close()
