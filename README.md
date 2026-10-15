[![CI](https://github.com/RaccoonlabDev/mini_v2_node/actions/workflows/ci.yml/badge.svg)](https://github.com/RaccoonlabDev/mini_v2_node/actions/workflows/ci.yml) [![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=RaccoonlabDev_mini_v2_node&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=RaccoonlabDev_mini_v2_node) [![Code Smells](https://sonarcloud.io/api/project_badges/measure?project=RaccoonlabDev_mini_v2_node&metric=code_smells)](https://sonarcloud.io/summary/new_code?id=RaccoonlabDev_mini_v2_node) [![LOC](https://sonarcloud.io/api/project_badges/measure?project=RaccoonlabDev_mini_v2_node&metric=ncloc)](https://sonarcloud.io/summary/new_code?id=RaccoonlabDev_mini_v2_node)

# CAN node firmware framework

A ready-to-use firmware framework for small STM32-based CAN nodes in robotics, like [RL Mini v2](https://docs.raccoonlab.co/guide/can_pwm/can_pwm_mini_v2.html) and [RL Mini v3](https://docs.raccoonlab.co/guide/can_pwm/mini_v3.html): read sensors, drive actuators and report their health over several CAN protocols.

<details open><summary><b>1. FEATURES</b></summary>

- [x] Multiple CAN protocol backends, selectable per firmware target — see [protocol backends](#protocol-backends)
- [x] CAN bootloader
- [x] Software-In-The-Loop (SITL) mode
- [x] Small firmware image, from ~42 KB on STM32F103, friendly for customization
- [x] Redundant CAN interface and CAN terminators control (node v3)
- [x] MPU-9250 IMU with vibration analysis (node v3)
- [x] Framework mode: keep your boards and modules in your own repository

</details>

<details open><summary><b>2. QUICK START</b></summary>

```bash
git clone git@github.com:RaccoonlabDev/mini_v2_node.git
cd mini_v2_node
pip install -r requirements.txt
./scripts/install.py

make rl_mini_v2_dronecan
# no toolchain? build in Docker:
scripts/docker_run.sh rl_mini_v2_dronecan
```

<details><summary>Bench connection example</summary>

<img src="https://raw.githubusercontent.com/wiki/RaccoonlabDev/mini_v2_node/assets/connection.png" alt="RL Mini v2 node wired to the RL programmer-sniffer over CAN and SWD" width="640">

</details>

**Upload/Flash**:

```bash
make rl_mini_v2_dronecan upload
```

**SITL (Software-In-The-Loop)**:

```bash
make rl_sitl_dronecan run
```

</details>

<details open><summary><b>3. TARGET HARDWARE</b></summary>

The software supports RL v2 nodes (stm32f103, 128 KBytes flash), v3 nodes (stm32g0b1, 512 KBytes flash) and v4 nodes (stm32h753, 2 MBytes flash, DroneCAN only, the hardware is not public yet).

| Name | View | Pinout |
| ---- | ---- | ------ |
| Mini v2 </br> stm32f103 </br> 128 KBytes flash | <img src="https://raw.githubusercontent.com/wiki/RaccoonlabDev/mini_v2_node/assets/mini_v2_view.png" alt="Mini v2" width="120"> | <img src="https://raw.githubusercontent.com/wiki/RaccoonlabDev/mini_v2_node/assets/mini_v2_pinout.png" alt="Mini v2 pinout" width="120"> |
| Mini v3 </br> stm32g0b1 </br> 512 KBytes flash | <img src="https://raw.githubusercontent.com/wiki/RaccoonlabDev/mini_v2_node/assets/mini_v3_view.png" alt="Mini v3" width="120"> | <img src="https://raw.githubusercontent.com/wiki/RaccoonlabDev/mini_v2_node/assets/mini_v3_pinout.png" alt="Mini v3 pinout" width="120"> |
| Node v4 </br> stm32h753 </br> 2 MBytes flash | not public yet | |

</details>

<details><summary><b>4. USE CASES</b></summary>

This node is a versatile CAN-connected controller integrating PWM control, IMU sensing, and dual CAN bus communication, suitable for robotics, automation, test rigs, and other embedded systems. Below are example use cases.

| Use case | Description |
|-|-|
| <img src="https://raw.githubusercontent.com/wiki/RaccoonlabDev/mini_v2_node/assets/use_case_servo.png" alt="A node driving a servo and an ESC" width="400"> | **1. Basic ESC and Servo Control** </br> PWM1-4 to control PWM ESCs via Cyphal/CAN or DroneCAN. </br> PWM1-2 to control 5V servos. </br> DroneCAN interface: [RawCommand](https://dronecan.github.io/Specification/7._List_of_standard_data_types/#rawcommand) / [ArrayCommand](https://dronecan.github.io/Specification/7._List_of_standard_data_types/#arraycommand). </br> Cyphal/CAN interface: [UDRAL setpoint](https://github.com/OpenCyphal/public_regulated_data_types/blob/master/reg/udral/service/actuator/esc/_.0.1.dsdl) |
| | **2. Advanced ESC Control with Feedback** </br> PWM3-4 for controlling PWM ESCs. </br> DroneCAN interface: [esc.Status](https://dronecan.github.io/Specification/7._List_of_standard_data_types/#status-2) / [actuator.Status](https://dronecan.github.io/Specification/7._List_of_standard_data_types/#status). </br> Cyphal/CAN interface: [UDRAL Feedback](https://github.com/OpenCyphal/public_regulated_data_types/blob/master/reg/udral/service/actuator/common/Feedback.0.1.dsdl) |
| <img src="https://raw.githubusercontent.com/wiki/RaccoonlabDev/mini_v2_node/assets/use_case_i2c.png" alt="A node reading an I2C sensor on its PWM pins" width="400"> | **3. CAN-I2C Converter** </br> PWM1 and PWM2 as I2C pins. </br> Cyphal/CAN or DroneCAN adapter for I2C peripherals. </br> Drivers to start from: SHT3x (humidity), INA228 (power). |
| | **4. End Effector / Servo Latch** </br> PWM1-2 to control a servo-based latch, clamp, or lightweight end effector. </br> Suitable for ground robots, mobile platforms, and general-purpose tooling. </br> DroneCAN interface: [hardpoint.Command](https://dronecan.github.io/Specification/7._List_of_standard_data_types/#command-1). |
| <img src="https://raw.githubusercontent.com/wiki/RaccoonlabDev/mini_v2_node/assets/vibrations_gui_tool.png" alt="The vibration metric plotted in gui_tool" width="400"> | **5. Vibration Analysis with IMU** (node v3) </br> Utilize the onboard IMU (MPU-9250). </br> Monitor vibrations in your system. </br> Estimate dominant frequency and magnitude of vibrations. </br> Useful for diagnosing mechanical issues </br> or ensuring smooth operation in robotic and embedded platforms. </br> DroneCAN interface: [ahrs.RawIMU](https://dronecan.github.io/Specification/7._List_of_standard_data_types/#rawimu). |
| | **6. Custom CAN Protocol** </br> A backend for your own CAN protocol, next to DroneCAN on the same bus. </br> `Src/modules/backends/raw_can` is an example that CI runs in SITL. |

</details>

<details><summary><b>5. SOFTWARE PREREQUISITES</b></summary>

The following table outlines the compatibility of the project with different operating systems, modes, and support timelines.

| **Operating System** | **DroneCAN Support** | **Cyphal Support** | **Notes**                              |
|----------------------|----------------------|--------------------|----------------------------------------|
| Ubuntu 26.04         | ✅ Supported         | ✅ Supported       | Latest Ubuntu LTS version, CI runs the SITL tests on it. |
| Ubuntu 24.04         | ✅ Supported         | ✅ Supported       | CI builds the firmware on it, the Docker image is based on it. |
| Ubuntu 22.04         | ⚠️ Not Maintained    | ⚠️ Not Maintained  | Not tested since mid-2026; unsupported in April 2027. |
| Windows (2022)       | ⚠️ Not Maintained    | ⚠️ Not Maintained  | May still work. |
| Manjaro (latest)     | ⚠️ Not Maintained    | ⚠️ Not Maintained  | No longer maintained by this project; may still work. |
| Ubuntu 20.04         | ❌ Not Supported     | ❌ Not Supported   | Deprecated in 2025-04-15.              |

Requirements:
- arm-none-eabi-gcc
- CMake
- Python
- STM32CubeMX/STM32CubeIDE, only to change a board's `.ioc`: builds download the generated HAL
- st-link
- GUI tools to talk to the node: [gui_tool](https://github.com/dronecan/gui_tool) for DroneCAN ([docs/mode_dronecan.md](docs/mode_dronecan.md)), [Yakut](https://github.com/OpenCyphal/yakut) / [Yukon](https://github.com/OpenCyphal/yukon) for Cyphal ([docs/mode_cyphal.md](docs/mode_cyphal.md))

</details>

<details><summary><b>6. ARCHITECTURE OVERVIEW</b></summary>

The project is organized into layers to make it portable across boards:

1. **Peripheral Layer ([Src/peripheral](Src/peripheral))**
    - STM32CubeMX-generated HAL code is wrapped in lightweight C++ classes.
    - High-level code accesses peripherals only through the `HAL` namespace.
    - Goal: minimize direct dependency on STM32CubeMX and ease migration to other platforms.
2. **Board Support / Platform Layer ([Src/boards](Src/boards), [Src/platform](Src/platform))**
    - Board-specific configuration (pins, ADC channels, LEDs, CAN terminators, etc.) and the firmware targets of each board.
    - One platform per MCU, and `ubuntu` for SITL.
3. **High-Level Layer**
    - **Drivers ([Src/drivers](Src/drivers))** — external devices (e.g. IMU).
    - **Modules ([Src/modules](Src/modules))** — application logic, e.g. vibration metrics.
    - **Backends ([Src/modules/backends](Src/modules/backends))** — one per CAN protocol: its core and a frontend per topic.
    - **Topics ([Src/topics](Src/topics))** — what modules exchange through the datapool.

**Build Targets**
- Build and flash using Make, for example: `make rl_mini_v3_dronecan`
- STM32CubeMX is used only to generate peripheral code. Application logic starts in `application_entry_point()` called from `main.c`.

**Standards & Hardware Info**
- Boards follow an internal standard for peripherals (ADC_VIN, ADC_5V, ADC_CURRENT, RGB LED, CAN_TERMINATORS, etc.), see [docs/hardware.md](docs/hardware.md).

### Protocol backends

At the CAN layer the firmware is protocol-agnostic — in principle any CAN protocol can be implemented. Backends currently provided, selectable per build target (`NC_TARGET`):

| Backend | Build target | Docs |
|-|-|-|
| DroneCAN | `dronecan` | [mode_dronecan.md](docs/mode_dronecan.md) |
| Cyphal/CAN | `cyphal` | [mode_cyphal.md](docs/mode_cyphal.md) |
| Both at once | `both` | |
| Raw CAN (an example, SITL only) | `raw_can` | [datapool.md](docs/datapool.md) |

A backend lives in `Src/modules/backends/<backend>/`: its `core` and one frontend per topic.

> Cyphal needs access to the private `libcpnode` repository, so CI skips it on forks.

### Build system

The firmware is built by **CMake** (out-of-source, into `build/`). CMake is the real build system: it selects the board, target, toolchain and sources.

The root **`Makefile` is only a thin convenience wrapper** around CMake. It does not build anything by itself — it just turns long, error-prone CMake invocations into short targets that are easy to type and **tab-complete** in the shell. So instead of:

```bash
cmake -S . -B build/.../obj -DNC_BOARD=rl/mini_v2 -DNC_TARGET=dronecan \
      -DNC_IMAGE_KIND=application -G "Unix Makefiles" && make -C build/.../obj
```

you type `make rl_mini_v2_dronecan`. Under the hood each target runs `scripts/build.sh`, which (re)configures CMake only when an input changed and then compiles. Final firmware binaries are copied to `release/`.

Naming rule:
- `<vendor>_<board>_<target>` builds the **application** image for the bootloader (`IMAGE_KIND=application`).
- `<vendor>_<board>_<target>_standalone` builds a **standalone** image explicitly.

Pass these as `VAR=value` before the target when you need them:

| Variable | Meaning |
| -------- | ------- |
| `CMAKE_BUILD_TYPE` | `Release` (default) or `Debug` |
| `NC_BUILD_DIR` | Build output root (default `<node-core>/build`) |
| `EXTRA_CMAKE_ARGS` | Extra `-D...` flags forwarded to CMake |

| Command | What it does |
| ------- | ------------ |
| `make all` | Builds the standard matrix of release targets |
| `make rl_mini_v2_dronecan upload` | Flashes the most recently built release `.bin` |
| `make rl_sitl_dronecan run` | Builds and runs the host SITL binary |
| `make tests` / `make code_style` | Runs unit tests / the linters and style checks |
| `make deps` | Clones the libraries ahead of a build, see [libraries](#libraries) |
| `make clean` / `make distclean` | Removes build objects / the whole `build/` tree |

The Makefile ends with optional, git-ignorable hooks, `Makefile.vendor` and `Makefile.local`: use them to add your own targets without editing the core Makefile.

### CubeMX HAL

CI can't run STM32CubeMX. So the generated HAL is produced once, archived, and restored at build time. Archives are keyed by the `.ioc` sha256 (`<name>-<iochash>.tar.gz`), so the HAL always matches its `.ioc`.

One script does it: [scripts/cubemx.py](scripts/cubemx.py), with `ensure` (consumer, runs in CI and builds) and `package` (producer, needs CubeMX). It reads a per-board manifest, `boards/<vendor>/<board>/cubemx.json`:

```json
{
  "name": "rl-mini-v3",
  "ioc": "board/project.ioc",
  "store": { "repo": "RaccoonlabDev/mini_v2_node", "tag": "cubemx-v0.1.0" }
}
```

`ensure` resolves in order: existing project → local cache → local CubeMX → download from the release.

<details><summary>Producing and publishing an archive</summary>

The archive is addressed by **(tag, `.ioc` hash)**: the asset name encodes the `.ioc`, the **`store.tag`** encodes the **generation environment** (STM32CubeMX version + `generate_cubemx_hal.sh`) — the part the hash can't see.

- Editing a `.ioc` → new hash, same tag (handled automatically).
- Upgrading CubeMX or changing the generator so the HAL output differs → **bump the tag**, regenerate, re-upload, update `store.tag`.
- Old commits keep their old `store.tag`, so they still restore matching HAL — builds stay reproducible. It is **not** the firmware version.

```bash
scripts/cubemx.py package --manifest <board dir>/cubemx.json --out build/cubemx/<vendor>/<board> --upload
```

Pass `--release-only` when a target must always consume the published project: Mini v3 does it so its IMU/DSP generation path can't slow ordinary builds. Make the Mini v3 archive self-contained before publishing it: copy the DSP implementation files used by `cmsisDspSources` from the ARM CMSIS pack selected by the `.ioc` into the matching directories under `Middlewares/Third_Party/ARM_CMSIS/CMSIS/DSP/Source/`.

</details>

### Libraries

[cmake/dependencies.cmake](cmake/dependencies.cmake) declares the libraries node-core builds on (libparams, libdcnode, libcpnode), one line each, with the version cloned at configure time. An application builds another version without a node-core change:

| CMake variable | Effect |
|---|---|
| `NC_<NAME>_VERSION` | Clone that tag or commit into `build/third_party/<name>-<version>` |
| `NC_<NAME>_PATH` | Build a local checkout as is, e.g. while fixing the library |
| `NC_THIRD_PARTY_DIR` | Where clones go, `build/third_party` by default |

```bash
EXTRA_CMAKE_ARGS="-DNC_LIBDCNODE_VERSION=a913f24" make rl_mini_v2_dronecan
```

`<NAME>` is `LIBPARAMS`, `LIBDCNODE` or `LIBCPNODE`. A build directory keeps the value, so pass an empty one to go back: `-DNC_LIBDCNODE_VERSION=`.

- The first build clones them, so it needs network once. Each version gets its own directory and a clone is never updated: switching versions needs no fetch, and an existing clone builds offline. Pin a tag or a commit; a branch is cloned once and stays where it was.
- Docker has no SSH key for the private libcpnode: run `make deps` on the host first, `scripts/docker_run.sh` mounts `build/`.
- On `main`, every pin is a commit on the main branch of its repository, so a squash merge or a deleted branch can't break it. [scripts/check_dependency_pins.py](scripts/check_dependency_pins.py) checks it in CI, and with `--repo` it checks an application too.

</details>

<details><summary><b>7. BASIC APPLICATION LOGIC & LED STATUS</b></summary>

All nodes run a minimal application after flashing so you can verify the board is working without extra setup:
- **CAN communication** starts according to the selected build target (DroneCAN or Cyphal/CAN).
- **LED indicator** follows a standard pattern common to all RaccoonLab boards — a convention typical for robotic applications:

| LED Pattern & Color | Meaning |
|-|-|
| **Blinking Blue/Red** | Initialization / Calibration       |
| **Blinking Yellow**   | Minor Failure / Warning            |
| **Blinking Magenta**  | Major Failure / Error              |
| **Blinking Red**      | Fatal Malfunction / Critical       |
| **Blinking Blue**     | Ready — No GPS Lock                |
| **Solid Blue**¹       | Engaged — No GPS Lock              |
| **Blinking Green**²   | Ready — 3D Fix (8+ satellites)     |
| **Solid Green**¹²     | Engaged — 3D Fix (8+ satellites)   |

¹ Applicable to nodes that support an **engaged** state (e.g. actuator nodes).

² Applicable to nodes that use **GPS** (e.g. GNSS or combined nodes).

</details>

<details><summary><b>8. CREATING YOUR OWN APPLICATION</b></summary>

This repository can be used in two ways:

| Approach | When to Use | Summary |
| -------- | ----------- | ------- |
| **Framework (recommended)** | When you build your own application. | Keep your custom boards, modules, and drivers in your app repo and build against `node-core` as a submodule. |
| **Fork** | When you contribute to `node-core` itself. | One fork per user/org, pull requests as usual. |

### Framework workflow

Use this workflow when your application lives in its own repository and this repo is included as a git submodule. You never edit `node-core`: boards, targets, modules and drivers live in your app and are layered on top.

```text
your_app/
├── Makefile                          # thin wrapper, one target per firmware
├── src/
│   ├── boards/<vendor>/<board>/
│   │   ├── board/board.cmake         # platform, cube project, flash layout
│   │   ├── cubemx.json               # your .ioc and where its HAL archive lives
│   │   └── <target>.cmake            # the firmware target
│   ├── modules/...                   # optional app modules
│   └── drivers/...                   # optional app drivers
└── third_party/node-core/            # submodule
```

1. **Board dir** `src/boards/<vendor>/<board>/board/board.cmake`. For existing hardware, copy a node-core board (e.g. `rl/mini_v2`) to reuse its `.ioc`/HAL as-is. For new hardware, add your own `.ioc` and `cubemx.json` — do not commit generated HAL, see [CubeMX HAL](#cubemx-hal).
2. **Target** `src/boards/<vendor>/<board>/<target>.cmake`. Start from a node-core target (e.g. `dronecan.cmake`) and `rl_include_module(...)` what you need.
3. **Node name**: `add_definitions(-DNC_DEFAULT_NODE_NAME="co.vendor.app.node")`.
4. **Makefile target** that forwards to `scripts/build.sh` with your sources in `--app-src-dir` (`NC_APP_SRC_DIR`).
5. Build it.

```make
my_board_dronecan:
	@bash third_party/node-core/scripts/build.sh --board my_vendor/my_board --target dronecan \
		--image-kind application --build-dir $(CURDIR)/build --app-src-dir $(CURDIR)/src
```

Good to know:
- A board in your app with the same path as a node-core board replaces it, so give new boards new names.
- Modules and drivers are looked up in your app first, then in node-core.
- Use `https://` URLs for public submodules and `git@` for private ones.

**Custom application versioning**
- Resulting version format: `v<upstream-version>-<app-version>`, e.g. `v1.3.0-0.1.0` (based on upstream `v1.3.0`, app version `0.1.0`).
- **Framework** (submodule): tag your app repo with the **app version only**, e.g. `v0.1.0`. The build derives the upstream prefix from the pinned `node-core` submodule and produces the combined `v<upstream>-<app>` automatically — so the artifact version always reflects the node-core you actually built against. Do **not** put the upstream version in the app tag (it would be duplicated).
- **Fork** (single repo): tag with the full combined version, e.g. `v1.3.0-0.1.0`.

</details>

<details><summary><b>9. Q&A</b></summary>

If you are struggling with the software building, please refer to the CI workflow [ci.yml](.github/workflows/ci.yml) for a hint. If it doesn't help, you can open [an issue](https://github.com/RaccoonlabDev/mini_v2_node/issues?q=is%3Aissue+).

Contributing: commit names, code style and the firmware rules are in [docs/CONTRIBUTING.md](docs/CONTRIBUTING.md), known bugs in [docs/issues.md](docs/issues.md).

</details>
