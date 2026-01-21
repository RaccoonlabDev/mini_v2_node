# Gimbal Application (DroneCAN)

## Overview
- **Use case**: Payload gimbal control and orientation feedback.
- **Protocol**: DroneCAN.
- **Hardware**: Mini v2 / Mini v3 with a PWM gimbal.

## Function Summary
Accepts gimbal commands over DroneCAN and drives PWM outputs for roll/pitch/yaw while publishing gimbal status feedback.

## Topics

### Published
| Topic | Rate | Notes |
| - | - | - |
| [uavcan.equipment.camera_gimbal.Status](https://dronecan.github.io/Specification/7._List_of_standard_data_types/#status-3) | 10 Hz | Enable with `feedback.gimbal.enable` (pending). |

Example message:
```text
gimbal_id: 0
mode:
  command_mode: 1 # ORIENTATION_FIXED_FRAME
camera_orientation_in_body_frame_xyzw: [-45.0000, 0.0000, 0.0000, 1.0000]
camera_orientation_in_body_frame_covariance: []
```

### Subscribed
| Topic | Notes |
| - | - |
| [uavcan.equipment.actuator.RawCommand](https://dronecan.github.io/Specification/7._List_of_standard_data_types/#rawcommand) | Select with `pwm.input_type`. Raw PWM values. |
| [uavcan.equipment.actuator.ArrayCommand](https://dronecan.github.io/Specification/7._List_of_standard_data_types/#arraycommand) | Select with `pwm.input_type`. Array of PWM values. |
| [uavcan.equipment.camera_gimbal.AngularCommand](https://dronecan.github.io/Specification/7._List_of_standard_data_types/#angularcommand) | Select with `pwm.input_type`. Roll/Pitch/Yaw commands. |

## Parameters
Typical parameters to set for gimbal control:

| Name | Default | Range | Description |
| - | - | - | - |
| `pwm.input_type` | - | - | Selects the active command input. |
| `pwm*.ch` | - | - | Channel mapping (roll/pitch/yaw for AngularCommand or channel index for raw/array). |
| `pwm*.min` | - | - | PWM duration at minimum command. |
| `pwm*.max` | - | - | PWM duration at maximum command. |
| `pwm*.def` | - | - | PWM duration when no command is available. |
| `feedback.gimbal.enable` | - | `0/1/2` | Publishes `camera_gimbal.Status` when enabled (pending). `0` disabled, `1` enabled (deg), `2` reserved (quaternion). |

## PWM Behavior
- **RawCommand**: `cmd` in `[0..8191]` maps linearly to `[pwm*.min..pwm*.max]`. Negative values select `pwm*.def`.
- **ArrayCommand**: `command_value` in `[-1.0..+1.0]` maps linearly to `[pwm*.min..pwm*.max]`. Out-of-range values select `pwm*.def`.
- **AngularCommand**: quaternion -> Euler (deg) -> normalize by `pwm.max_servos_angle / 2` and clamp to `[-1.0..+1.0]`, then map to `[pwm*.min..pwm*.max]`.

## Configuration Steps
1. Set `pwm.input_type` to the desired command source.
2. Map `pwm*.ch` to roll/pitch/yaw (AngularCommand) or to raw/array indices.
3. Set `pwm*.min`, `pwm*.max`, and `pwm*.def` to match the gimbal PWM range.
4. Enable `feedback.gimbal.enable` when available to publish status.

## Notes
- Hardpoint and arming inputs are available but typically not used for gimbals.
