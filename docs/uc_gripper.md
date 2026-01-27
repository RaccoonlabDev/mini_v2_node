# USE CASE — CARGO GRIPPER

## 1. SUMMARY

Use PWM outputs to drive a servo-based gripper and control it over DroneCAN using the standard `hardpoint.Command` message for payload handling.

## 2. SETUP

Connect the gripper servo to PWM1 or PWM2 (5V servo outputs). No additional hardware is required beyond a suitable servo and power.

## 3. PARAMETERS / REGISTERS (ESSENTIAL ONLY)

| Param | Values | Meaning |
| ----- | ------ | ------- |
| pwm.cmd_type | 0..2 | Selects command type. Use `2` for `hardpoint.Command`. |
| pwm1.ch | [-1; 255] | Setpoint channel index for PWM1 (use `-1` to disable). |
| pwm1.min / pwm1.max / pwm1.def | µs | Servo pulse widths for min/max/default positions. |
| pwm2.ch | [-1; 255] | Setpoint channel index for PWM2 (use `-1` to disable). |
| pwm2.min / pwm2.max / pwm2.def | µs | Servo pulse widths for min/max/default positions. |

## 4. PUBLISHERS / SUBSCRIBERS (ESSENTIAL ONLY)

**Publishers (outgoing):**
| Data type | Notes |
| --------- | ----- |
| — | None required for basic control. |

**Subscribers (incoming):**
| Data type | Notes |
| --------- | ----- |
| [hardpoint.Command](https://dronecan.github.io/Specification/7._List_of_standard_data_types/#command-1) | Use to open/close or set the gripper position. |

## 5. USAGE

Set `pwm.cmd_type=2`, map PWM1/PWM2 channels to the desired setpoint index, and tune min/max/default pulse widths for your servo. Then send `hardpoint.Command` messages and observe the gripper movement.

## 6. NOTES

Servo current draw can be significant; ensure adequate power. If the gripper oscillates or chatters, widen the deadband via PWM min/max tuning or lower update rate on the sender side.
