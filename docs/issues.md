# Issues

## ISSUE-0001
- affected versions: [v1.4.0, v1.5.0]
- introduced by: `b65b63c` (`pwm: introduce pwm router and refactor pwm frontends`, 2025-12-08)
- on test: `Tests/test_issue_0001_pwm_same_channel_apply.py`

## ISSUE-0002
- affected versions: [v3 introduction, v1.6.0)
- introduced by: STM32G0 (Mini v3) support
- symptoms: stm32g0 flashing fails every second attempt with `st-flash ... Flash programming error: 0x00000080`
- root cause: flashing without connect-under-reset; target may be running IWDG and reset during programming
- fixed in: v1.6.0 by using `--connect-under-reset` in `scripts/flash.sh`
