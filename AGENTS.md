# Agent Instructions

- Build system: [docs/build_system.md](docs/build_system.md).
- When using as a framework (submodule, custom boards/targets), follow [docs/framework.md](docs/framework.md).
- Prebuilt CubeMX HAL archives (CI without CubeMX): [docs/cubemx.md](docs/cubemx.md).
- Firmware contract: [docs/embedded_firmware_contract.md](docs/embedded_firmware_contract.md).
- Commit naming and code style: [docs/CONTRIBUTING.md](docs/CONTRIBUTING.md).
- When writing docs, follow [docs/documentation_guidelines.md](docs/documentation_guidelines.md).
- Treat Git as read-only: you may run inspection commands like `git status`,
  `git diff`, and `git log`, but never modify Git state, including
  staging/unstaging files, committing, branching, rebasing, resetting, checking
  out, stashing, tagging, or changing config.
- Never touch remotes: no `git push`, `git pull`, `git fetch`, or any
  equivalent through other tools (`gh`, scripts, CI triggers). This applies to
  every branch, including the current one. Pushing is done only by a human,
  ever.
