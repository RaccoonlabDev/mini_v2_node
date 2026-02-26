# Python Test Guide

## Tools
- Prefer `pytest` + plugin `pytest-dependency` over `unittest`
- Recommended libs: [`dronecan` (PyPI)](https://pypi.org/project/dronecan/), [`pycyphal` (PyPI)](https://pypi.org/project/pycyphal/)

## Structure
- Keep tests small and focused.
- Use staged tests with `pytest-dependency`.
- Required first stages for communication/integration suites:
  1. `test_*_interface_exists`
  2. `test_*_single_node_online`
- After these two, add as many behavior/setup checks as needed. Do not force a fixed number of tests.
- Keep each test easy to scan. Ideally, one test fits on one screen.

## Node and Interface Rules
- Reuse a single node instance in one test module unless multiple instances are required.
- Before first service call, wait for `NodeStatus` on that node instance.
- Interface selection order:
  1. `slcan0` (SITL/virtual)
  2. `/dev/serial/by-id/usb-STMicroelectronics_STM32_STLink_*-if02`
- Use `slcan:<device_path>` for serial SLCAN; pass bitrate via node constructor args.

## Reliability
- Use fixed random seed for reproducibility.
- For setup/service calls (e.g., parameter writes):
  - retry 3-5 times
  - use short timeout per attempt
  - add a small gap between consecutive calls
  - print retry attempts on failure

## Output and Warnings
- Default useful output in `pytest.ini`:
  - `addopts = -s`
- Filter out-of-scope warnings in `pytest.ini`.
- Keep noisy third-party loggers at `WARNING` when needed.

## Failure Messages
- Fail early with clear messages for infra issues (no iface, no node, too many nodes).
- Keep behavior assertions strict; avoid broad retries that can hide logic regressions.
