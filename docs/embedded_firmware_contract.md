# Embedded Firmware Contract

Prefer predictable, bounded, explicit code over convenience abstractions.

## Default Rules

- Do not use heap memory in firmware code unless explicitly approved for that
  specific place.
- Avoid APIs that hide heap allocation, such as `std::vector`, `std::string`,
  `std::map`, `std::function`, Arduino `String`, `new`, `delete`, `malloc`, and
  `free`.
- Prefer fixed-size storage: arrays, `std::array`, static buffers, and
  compile-time capacities.
- Keep execution time bounded. Avoid unbounded loops, blocking waits, sleeps, and
  retries in control paths.
- Validate all external input before use: CAN/DroneCAN/Cyphal messages,
  registers, parameters, sensor readings, and onboard/ROS2 bridge data.
- Make units and ranges explicit for hardware-facing values.
- Treat actuator outputs as safety-relevant. Define the safe/default state and
  preserve it on invalid input, timeout, or communication loss.
- Do not silently change wire formats, register meanings, node IDs, topic names,
  or documented device behavior.
- Do not hand-edit generated files. Change the source schema/configuration and
  regenerate.
- Keep dependencies minimal. Do not add libraries or runtime features without a
  clear embedded reason.
- Keep logging and diagnostics bounded; avoid noisy or blocking diagnostics in
  time-sensitive paths.
- Preserve existing build, simulation, formatting, and documentation workflows.

## When In Doubt

- Prefer a small explicit implementation over a generic abstraction.
- Prefer compile-time configuration over runtime discovery.
- Prefer failing safe over continuing with uncertain state.
- Ask before introducing memory allocation, protocol changes, or new background
  execution models.
