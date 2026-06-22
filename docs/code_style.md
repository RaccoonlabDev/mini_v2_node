# Code style

We use [PX4-Autopilot code style](https://docs.px4.io/main/en/contribute/code), with the following (minimal) modifications:

- Indentation: use 4 spaces instead of 8-space Tabs
- Attach a pointer or reference operator (*, &) to the variable type (left)
- Use UPPER_CASE for constants names instead of kThisIsConstant

```
indent=spaces=4
align-pointer=type
align-reference=type
```

## How to use?

```bash
make check_format
```
