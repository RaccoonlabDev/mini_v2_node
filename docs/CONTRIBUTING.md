# Contributing

## Commits and pull requests

Pull requests are squash-merged, so the pull request title becomes the commit
subject. One convention covers both:

```
<type>(<scope>): <summary>
```

- Write the summary in the imperative, lower case, with no trailing dot.
- `<scope>` is optional and names the affected part: `fix(spi):`, `feat(params):`.
- A scope is not a type: write `fix(rcout):`, not `rcout:`.

| Type | Use for |
|---|---|
| `feat` | User-visible feature or new build target |
| `fix` | Bug fix or behaviour correction |
| `docs` | Documentation, including code comments |
| `tests` | Tests, test data, test helpers |
| `refactor` | Restructuring without intended behaviour change |
| `ci` | Workflows, checks, automation |
| `build` | Build system, CMake, release artifacts |
| `chore` | Dependencies, submodules, repository maintenance |

## Code style

We use [PX4-Autopilot code style](https://docs.px4.io/main/en/contribute/code),
with the following (minimal) modifications:

- Indentation: use 4 spaces instead of 8-space Tabs
- Attach a pointer or reference operator (*, &) to the variable type (left)
- Use UPPER_CASE for constants names instead of kThisIsConstant

```
indent=spaces=4
align-pointer=type
align-reference=type
```

```bash
make code_style
```

## Firmware rules

Prefer predictable, bounded, explicit code over convenient abstractions.

- No heap: no `new`, `delete` or `malloc`, and none of `std::vector`,
  `std::string`, `std::map` or `std::function`, which allocate. Use arrays,
  `std::array` and static buffers sized at compile time.
- Keep execution time bounded: no unbounded loops, blocking waits, sleeps or
  retries in control paths.
- Validate what comes from outside before using it: CAN messages, parameters,
  sensor readings. Make units and ranges explicit.
- Treat actuator outputs as safety-relevant: define the safe state, and keep it
  on invalid input, a timeout or lost communication.
- Don't silently change wire formats, parameter meanings, node IDs or topic names.
- Don't edit generated files: change their source and regenerate.
- Don't add a library without a clear embedded reason.
- Don't use `sprintf`/`snprintf` on STM32F103 and other small targets: newlib's formatter adds ~3 KB of flash and links `malloc`, so log a fixed string instead.
- Ask before adding heap use, a protocol change or a new background task.

## Tests

Unit tests live in `Tests/` and run with `make tests`. Integration tests are
Python, run against SITL or a node behind a CAN sniffer:

- Use `pytest` with `pytest-dependency`, and the `dronecan` or `pycyphal` packages.
- Start a suite with `test_*_interface_exists` and `test_*_single_node_online`,
  then add as many checks as needed, each one short.
- Pick the interface in this order: `slcan0` (SITL), then
  `/dev/serial/by-id/usb-STMicroelectronics_STM32_STLink_*-if02` as `slcan:<path>`.
- Reuse one node per test module, and wait for a `NodeStatus` before the first
  service call: allow more than its 1 s period.
- Use a fixed random seed. Retry setup calls such as parameter writes 3-5 times
  with short timeouts, but keep behaviour assertions strict.
- Fail early with a clear message when the setup is wrong: no interface, no node,
  more than one node.

## Before opening a pull request

- Build every target you touched: see the build system in the [README](../README.md#build-system).
- Run `make code_style` and the tests.
- Open the pull request to get a CI run. A branch without one is built only on
  demand, from the Actions tab.
- Review the firmware size comment CI posts on the pull request, and say in the
  description why anything grew.

## Writing docs

Docs here are read by busy humans, and more and more through an assistant that
also reads the code. Keep them short, and don't restate what the code says.

- The [README](../README.md) is the entry point: what a user needs, with long
  parts in collapsed sections.
- Add a separate file only for what doesn't fit there: these rules, a design the
  code doesn't show at a glance ([datapool.md](datapool.md)), or a list the tests
  refer to ([issues.md](issues.md)).
- Lead with what the reader does. Cut history and rationale unless they change an action.
- Prefer a table, a command or a short list over prose, and show a working example.
