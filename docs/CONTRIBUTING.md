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

## Before opening a pull request

- Build every target you touched: [build_system.md](build_system.md).
- Run `make code_style` and the tests: [python_test_guide.md](python_test_guide.md).
- Fill in the firmware size difference in the pull request template.

## Writing docs

Keep docs short and follow [documentation_guidelines.md](documentation_guidelines.md).
Known bugs and the tests covering them live in [issues.md](issues.md).
