# Framework usage

Use node-core as a git submodule and keep your product code in your own repo.
You never edit node-core: boards, targets, modules and drivers live in your app
and are layered on top. See also the README "Framework" workflow and
[build_system.md](build_system.md).

## Layout

```text
your_app/
├── Makefile                          # thin wrapper, one target per firmware
├── src/
│   ├── boards/<vendor>/<board>/
│   │   ├── board/board.cmake         # platform, cube project, flash layout
│   │   └── <target>.cmake            # the firmware target
│   ├── modules/...                   # optional app modules
│   └── drivers/...                   # optional app drivers
└── third_party/node-core/            # submodule
```

Point the build at `src/` with `--app-src-dir` (`NC_APP_SRC_DIR`).

Submodule URL scheme — pick by visibility:
- **Public** submodule → `https://github.com/...` so CI and fresh clones work without an SSH key.
- **Private** submodule → `git@github.com:...` so developers clone with their key (CI then needs a deploy key or token).

## Add a node (proven recipe)

1. **Board dir** `src/boards/<vendor>/<board>/board/board.cmake`. For existing
   hardware, copy a node-core board (e.g. `rl/mini_v2`) and keep
   `STM32_CUBEMX_PROJECT_PATH` pointing at node-core's stock cube project to
   reuse the `.ioc`/HAL as-is. For new hardware, add your own `.ioc` and
   generate the HAL with your script — do not commit generated HAL. For CI, use
   prebuilt HAL archives: see [cubemx.md](cubemx.md).
2. **Target** `src/boards/<vendor>/<board>/<target>.cmake`. Start from a
   node-core target (e.g. `dronecan.cmake`) and `rl_include_module(...)` what
   you need.
3. **Node name**: `add_definitions(-DNC_DEFAULT_NODE_NAME="co.vendor.app.node")`.
4. **Makefile target** that forwards to `scripts/build.sh` (copy an existing one).
5. Build it.

## Rules that bite

- A board dir is self-contained and **fully shadows** a core board of the same
  path — it does not merge. Give new boards a new name.
- Modules and drivers resolve **app-first, then core** via `rl_include_module` /
  `rl_include_driver`. To override a core module, copy the whole module dir.
- A module's `CMakeLists.txt` may pull a driver via
  `include(${SRC_DIR}/drivers/...)`, which bypasses the app override. If you
  override such a driver, change that line to `rl_include_driver(<driver>)`.
