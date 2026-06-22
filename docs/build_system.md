# Build system: CMake + Makefile

## 1. Purpose

The firmware is built by **CMake** (out-of-source, into `build/`). CMake is the
real build system: it selects the board, target, toolchain and sources.

The root **`Makefile` is only a thin convenience wrapper** around CMake. It does
not build anything by itself — it just turns long, error-prone CMake invocations
into short targets that are easy to type and **tab-complete** in the shell. This
is the same idea as `PX4-Autopilot`, where `make px4_fmu-v5_default` hides a full
CMake configure+build.

So instead of:

```bash
cmake -S . -B build/.../obj -DNC_BOARD=rl/mini_v2 -DNC_TARGET=dronecan \
      -DNC_IMAGE_KIND=application -G "Unix Makefiles" && make -C build/.../obj
```

you type:

```bash
make rl_mini_v2_dronecan
```

## 2. Building

Each board/target combination has a ready-made target you can tab-complete:

```bash
make rl_mini_v2_dronecan       # bootloader-linked application image
make rl_mini_v3_cyphal         # cyphal app for Mini v3
make rl_sitl_dronecan          # host SITL build
```

Naming rule:
- `<vendor>_<board>_<target>` builds the **application** image (`IMAGE_KIND=application`).
- `<vendor>_<board>_<target>_standalone` builds a **standalone** image explicitly.

Under the hood each target runs `scripts/build.sh`, which (re)configures CMake
only when an input changed and then compiles.

## 3. Options

Pass these as `VAR=value` before the target when you need them:

| Variable | Meaning |
| -------- | ------- |
| `CMAKE_BUILD_TYPE` | `Release` (default) or `Debug` |
| `NC_BUILD_DIR` | Build output root (default `<node-core>/build`) |
| `EXTRA_CMAKE_ARGS` | Extra `-D...` flags forwarded to CMake |

Example: `CMAKE_BUILD_TYPE=Debug make rl_mini_v2_dronecan`

## 4. Common commands

| Command | What it does |
| ------- | ------------ |
| `make` | Prints help (no default build target) |
| `make all` | Builds the standard matrix of release targets |
| `make rl_mini_v2_dronecan upload` | Flashes the most recently built release `.bin` |
| `make rl_sitl_dronecan run` | Builds and runs the host SITL binary |
| `make tests` | Runs unit tests |
| `make code_style` | Runs the linters / style checks |
| `make clean` / `make distclean` | Removes build objects / the whole `build/` tree |

## 5. Extending it

The Makefile ends with optional, git-ignorable hooks:

```make
-include ${ROOT_DIR}/Makefile.vendor
-include ${ROOT_DIR}/Makefile.local
```

Use these to add your own targets without editing the core Makefile. When this
repo is consumed as a framework, your application keeps its own top-level
Makefile that forwards to `node-core`; see the README "Framework" workflow.
