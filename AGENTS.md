# Agent Instructions

- Layers, build system, CubeMX HAL and libraries: README.md, section 6 ([build system](README.md#build-system), [libraries](README.md#libraries)).
- When using as a framework (submodule, custom boards/targets), follow [README.md, framework workflow](README.md#framework-workflow).
- Commit naming, code style, firmware rules, tests and docs: [docs/CONTRIBUTING.md](docs/CONTRIBUTING.md).
- Treat Git as read-only: you may run inspection commands like `git status`,
  `git diff`, and `git log`, but never modify Git state, including
  staging/unstaging files, committing, branching, rebasing, resetting, checking
  out, stashing, tagging, or changing config.
- Never touch remotes: no `git push`, `git pull`, `git fetch`, or any
  equivalent through other tools (`gh`, scripts, CI triggers). This applies to
  every branch, including the current one. Pushing is done only by a human,
  ever.
