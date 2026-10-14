#!/usr/bin/env python3
"""SITL check of the raw CAN example frontend (Src/modules/raw_can/board_status).

make rl_sitl_raw_can create_slcan0
./build/rl_sitl_raw_can_sitl/obj/node &
pytest -s Tests/test_raw_can_board_status.py
"""
import os
import socket
import struct
import time

import pytest
pytest.importorskip("pytest_dependency", reason="Install pytest-dependency to run dependency-gated tests.")

IFACE = "slcan0"
FRAME_ID = 0x100 + 42           # raw_can::BOARD_STATUS_BASE_ID + the SITL node id
PUBLISH_RATE_HZ = 2.0           # BoardStatusModule
CAPTURE_S = 5.0
CAN_FRAME = struct.Struct("<IB3x8s")
CAN_EFF_FLAG = 0x80000000
CENTIAMPS_NOT_AVAILABLE = -0x8000
BOARD_ERROR_OVERHEAT = 1 << 5   # SITL takes the host CPU temperature, which may run hot


def capture(seconds):
    frames = []
    with socket.socket(socket.AF_CAN, socket.SOCK_RAW, socket.CAN_RAW) as sock:
        sock.bind((IFACE,))
        sock.settimeout(0.1)
        end = time.monotonic() + seconds
        while time.monotonic() < end:
            try:
                can_id, dlc, data = CAN_FRAME.unpack(sock.recv(CAN_FRAME.size))
            except socket.timeout:
                continue
            if not can_id & CAN_EFF_FLAG and can_id == FRAME_ID:
                frames.append((dlc, data))
    return frames


@pytest.fixture(scope="module")
def frames():
    return capture(CAPTURE_S)


@pytest.mark.dependency()
def test_raw_can_interface_exists():
    assert os.path.exists(f"/sys/class/net/{IFACE}"), f"No {IFACE}: run `make create_slcan0`"


@pytest.mark.dependency(depends=["test_raw_can_interface_exists"])
def test_raw_can_single_node_online(frames):
    assert frames, f"No frame with id {FRAME_ID:#x}: is rl_sitl_raw_can running?"


@pytest.mark.dependency(depends=["test_raw_can_single_node_online"])
def test_raw_can_rate(frames):
    expected = PUBLISH_RATE_HZ * CAPTURE_S
    assert abs(len(frames) - expected) <= 2, f"{len(frames)} frames in {CAPTURE_S} s"


@pytest.mark.dependency(depends=["test_raw_can_single_node_online"])
def test_raw_can_payload(frames):
    for dlc, data in frames:
        v5_mv, vin_mv, current, celsius, flags = struct.unpack("<HHhbB", data)
        assert dlc == 8
        assert flags & ~BOARD_ERROR_OVERHEAT == 0, f"error flags {flags:#x} with the rails in range"
        assert -40 <= celsius <= 125, f"temperature {celsius} °C"
        assert 4200 <= v5_mv <= 5500, f"5 V rail: {v5_mv} mV"
        assert 4500 <= vin_mv <= 60000, f"Vin: {vin_mv} mV"
        assert current == CENTIAMPS_NOT_AVAILABLE, "the SITL board measures no current"
