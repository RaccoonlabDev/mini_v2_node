#!/usr/bin/env python3
import glob
import logging
import multiprocessing
import os
import random
import time

import dronecan
import pytest
pytest.importorskip("pytest_dependency", reason="Install pytest-dependency to run dependency-gated tests.")

# Keep test output clean: hide verbose SLCAN init/info logs.
logging.getLogger("dronecan.driver.slcan").setLevel(logging.WARNING)
pytestmark = pytest.mark.filterwarnings(
    r"ignore:This process .* is multi-threaded, use of fork\(\) may lead to deadlocks in the child\.:DeprecationWarning"
)

TARGET_NODE_ID_ENV = "MINI_NODE_ID"
TESTER_NODE_ID = 120
CAN_BITRATE = 1_000_000
NODE_STATUS_DISCOVERY_TIMEOUT_S = 1.5
STATUS_TIMEOUT_S = 0.1
SPIN_STEP_S = 0.005
PARAM_SET_ATTEMPTS = 5
PARAM_SET_TIMEOUT_S = 0.4
PARAM_SET_GAP_S = 0.03
RANDOM_SEED = 1001
PARAM_SETUP_PRIME_TIMEOUT_S = 0.8

SELECTED_IFACE = None
DISCOVERED_TARGET_NODE_ID = None
SHARED_NODE = None

random.seed(RANDOM_SEED)
SHARED_CHANNEL = random.randint(0, 19)
RAW_VALUE_1 = random.randint(1, 8191)
RAW_VALUE_2 = random.randint(1, 8191)

PWM_CHANNELS = (1, 2, 3, 4)
PWM_LIMITS = {}
for pwm_idx in PWM_CHANNELS:
    low, high = sorted(random.sample(range(1000, 2001), 2))
    PWM_LIMITS[pwm_idx] = {
        "min": low,
        "max": high,
        "def": random.randint(1000, 2000),
    }


def _select_iface():
    # 1) Prefer virtual CAN netdev in SITL mode.
    if os.path.exists("/sys/class/net/slcan0"):
        return "slcan0"

    # 2) Fallback to real STLink serial endpoint for SLCAN.
    candidates = sorted(glob.glob("/dev/serial/by-id/usb-STMicroelectronics_STM32_STLink_*-if02"))
    if candidates:
        return f"slcan:{candidates[0]}"

    return None


def _make_node(iface: str) -> dronecan.node.Node:
    try:
        _ = multiprocessing.Lock()
    except PermissionError as exc:
        pytest.skip(f"Cannot create multiprocessing semaphore in this environment: {exc}")

    try:
        node = dronecan.make_node(
            iface,
            node_id=TESTER_NODE_ID,
            bitrate=CAN_BITRATE,
            baudrate=CAN_BITRATE,
        )
    except PermissionError as exc:
        pytest.skip(f"Cannot create DroneCAN node in this environment: {exc}")

    node.mode = dronecan.uavcan.protocol.NodeStatus().MODE_OPERATIONAL
    node.health = dronecan.uavcan.protocol.NodeStatus().HEALTH_OK
    return node


def _discover_online_node_ids(node: dronecan.node.Node, timeout_s: float):
    seen = set()

    def _on_status(event):
        if event is None:
            return
        src = event.transfer.source_node_id
        if src is None or src == TESTER_NODE_ID:
            return
        seen.add(int(src))

    node.add_handler(dronecan.uavcan.protocol.NodeStatus, _on_status)
    deadline = time.monotonic() + timeout_s
    while time.monotonic() < deadline:
        node.spin(SPIN_STEP_S)

    return seen


def _set_int_param(node: dronecan.node.Node, target_node_id: int, name: str, value: int) -> bool:
    last_error = None

    for attempt_idx in range(PARAM_SET_ATTEMPTS):
        req = dronecan.uavcan.protocol.param.GetSet.Request()
        req.name = name
        req.value = dronecan.uavcan.protocol.param.Value(integer_value=int(value))

        response = {"event": None}

        def _on_response(event):
            response["event"] = event

        node.request(req, target_node_id, _on_response)
        deadline = time.monotonic() + PARAM_SET_TIMEOUT_S
        while response["event"] is None and time.monotonic() < deadline:
            node.spin(SPIN_STEP_S)

        if response["event"] is not None:
            # Add a small gap between parameter writes to avoid back-to-back bursts.
            node.spin(PARAM_SET_GAP_S)
            return attempt_idx == 0

        last_error = f"Timeout setting param {name}"
        print(
            f"[param-retry] {name} attempt {attempt_idx + 1}/{PARAM_SET_ATTEMPTS} failed "
            f"(timeout {PARAM_SET_TIMEOUT_S:.2f}s)",
            flush=True,
        )
        node.spin(PARAM_SET_GAP_S)

    assert False, f"{last_error} after {PARAM_SET_ATTEMPTS} attempts"


def _wait_target_node_status(node: dronecan.node.Node, target_node_id: int, timeout_s: float) -> bool:
    seen = {"ok": False}

    def _on_status(event):
        if event is None:
            return
        if event.transfer.source_node_id == target_node_id:
            seen["ok"] = True

    node.add_handler(dronecan.uavcan.protocol.NodeStatus, _on_status)
    deadline = time.monotonic() + timeout_s
    while not seen["ok"] and time.monotonic() < deadline:
        node.spin(SPIN_STEP_S)

    return seen["ok"]


def _build_raw_command(required_channel: int, value: int):
    msg = dronecan.uavcan.equipment.esc.RawCommand()
    msg.cmd = [0] * (required_channel + 1)
    msg.cmd[required_channel] = int(value)
    return msg


def _expected_percent(raw_value: int) -> int:
    # Mirror firmware logic:
    # command -> PWM us (uint16 cast) -> percent (uint8 cast).
    pwm_us = int(1000.0 + (float(raw_value) / 8191.0) * 1000.0)
    percent = int(((pwm_us - 1000) * 100.0) / 1000.0)
    return max(0, min(100, percent))


def _collect_status_samples(node: dronecan.node.Node, target_node_id: int, actuator_channel: int, timeout_s: float):
    samples = []

    def _on_status(event):
        if event is None:
            return
        if event.transfer.source_node_id != target_node_id:
            return
        msg = event.message
        if int(msg.esc_index) != actuator_channel:
            return
        samples.append(int(msg.power_rating_pct))

    node.add_handler(dronecan.uavcan.equipment.esc.Status, _on_status)
    deadline = time.monotonic() + timeout_s
    while time.monotonic() < deadline:
        node.spin(SPIN_STEP_S)

    return samples


@pytest.mark.dependency(name="pwm_same_channel_apply_iface")
def test_pwm_same_channel_apply_interface_exists():
    global SELECTED_IFACE
    SELECTED_IFACE = _select_iface()
    assert SELECTED_IFACE is not None, (
        "No CAN interface found. Expected either 'slcan0' or "
        "/dev/serial/by-id/usb-STMicroelectronics_STM32_STLink_*-if02"
    )


@pytest.mark.dependency(name="pwm_same_channel_apply_single_node", depends=["pwm_same_channel_apply_iface"])
def test_pwm_same_channel_apply_single_node_online():
    global DISCOVERED_TARGET_NODE_ID, SHARED_NODE
    assert SELECTED_IFACE is not None, "Interface is not initialized by dependency test."
    SHARED_NODE = _make_node(SELECTED_IFACE)
    online_node_ids = sorted(_discover_online_node_ids(SHARED_NODE, NODE_STATUS_DISCOVERY_TIMEOUT_S))

    assert online_node_ids, (
        f"No NodeStatus received within {NODE_STATUS_DISCOVERY_TIMEOUT_S:.1f}s on {SELECTED_IFACE}. "
        "Start one target node and retry."
    )
    assert len(online_node_ids) == 1, (
        f"Expected exactly one online target node, got {len(online_node_ids)}: {online_node_ids}. "
        "Leave only one target node on CAN bus and retry."
    )

    DISCOVERED_TARGET_NODE_ID = online_node_ids[0]
    configured = os.getenv(TARGET_NODE_ID_ENV)
    if configured is not None:
        assert DISCOVERED_TARGET_NODE_ID == int(configured), (
            f"Discovered node ID {DISCOVERED_TARGET_NODE_ID} does not match "
            f"{TARGET_NODE_ID_ENV}={configured}."
        )


@pytest.mark.dependency(name="pwm_same_channel_apply_setup", depends=["pwm_same_channel_apply_single_node"])
def test_pwm_same_channel_apply_target_node_setup():
    node = SHARED_NODE
    target_node_id = DISCOVERED_TARGET_NODE_ID

    # Prime communication before first param write.
    assert _wait_target_node_status(node, target_node_id, PARAM_SETUP_PRIME_TIMEOUT_S), (
        f"Did not receive NodeStatus from target node {target_node_id} within "
        f"{PARAM_SETUP_PRIME_TIMEOUT_S:.1f}s before parameter setup."
    )
    node.spin(0.1)

    all_first_attempt_ok = True

    all_first_attempt_ok &= _set_int_param(node, target_node_id, "pwm.cmd_ttl_ms", 1000)
    all_first_attempt_ok &= _set_int_param(node, target_node_id, "pwm.input_type", 0)
    all_first_attempt_ok &= _set_int_param(node, target_node_id, "feedback.esc.enable", 1)

    for pwm_idx in PWM_CHANNELS:
        # Apply mapping under test: all physical outputs follow one actuator channel.
        all_first_attempt_ok &= _set_int_param(node, target_node_id, f"pwm{pwm_idx}.ch", SHARED_CHANNEL)

        # Randomized limits (deterministic by seed) keep the setup compact and robust.
        all_first_attempt_ok &= _set_int_param(node, target_node_id, f"pwm{pwm_idx}.min", PWM_LIMITS[pwm_idx]["min"])
        all_first_attempt_ok &= _set_int_param(node, target_node_id, f"pwm{pwm_idx}.max", PWM_LIMITS[pwm_idx]["max"])
        all_first_attempt_ok &= _set_int_param(node, target_node_id, f"pwm{pwm_idx}.def", PWM_LIMITS[pwm_idx]["def"])

    if all_first_attempt_ok:
        print("\n[param-setup] all params set successfully on first attempt", flush=True)

    # Let params settle and flush stale traffic before primary behavior check.
    node.spin(0.05)


@pytest.mark.dependency(depends=["pwm_same_channel_apply_setup"])
def test_pwm_same_channel_apply_rawcommand_apply_all_mapped_outputs():
    node = SHARED_NODE
    target_node_id = DISCOVERED_TARGET_NODE_ID
    tol = 1
    mapped_outputs = len(PWM_CHANNELS)
    skip_samples = mapped_outputs
    check_samples = 2 * mapped_outputs
    collect_timeout_s = STATUS_TIMEOUT_S * 3

    node.broadcast(_build_raw_command(SHARED_CHANNEL, RAW_VALUE_1))
    samples_1 = _collect_status_samples(node, target_node_id, SHARED_CHANNEL, collect_timeout_s)
    checked_1 = samples_1[skip_samples:skip_samples + check_samples]
    expected_1 = _expected_percent(RAW_VALUE_1)
    assert samples_1, "No esc.Status samples received for the requested channel."
    assert len(checked_1) == check_samples, (
        f"Too few esc.Status samples after skipping first {skip_samples}: "
        f"got total {len(samples_1)}, required >= {skip_samples + check_samples}. Raw: {samples_1}"
    )
    assert all(abs(sample - expected_1) <= tol for sample in checked_1), (
        f"Unexpected esc.Status power_rating_pct values: {checked_1}, expected ~{expected_1}. Raw: {samples_1}"
    )

    node.broadcast(_build_raw_command(SHARED_CHANNEL, RAW_VALUE_2))
    samples_2 = _collect_status_samples(node, target_node_id, SHARED_CHANNEL, collect_timeout_s)
    checked_2 = samples_2[skip_samples:skip_samples + check_samples]
    expected_2 = _expected_percent(RAW_VALUE_2)
    assert samples_2, "No esc.Status samples received for the requested channel."
    assert len(checked_2) == check_samples, (
        f"Too few esc.Status samples after skipping first {skip_samples}: "
        f"got total {len(samples_2)}, required >= {skip_samples + check_samples}. Raw: {samples_2}"
    )
    assert all(abs(sample - expected_2) <= tol for sample in checked_2), (
        f"Unexpected esc.Status power_rating_pct values: {checked_2}, expected ~{expected_2}. Raw: {samples_2}"
    )
