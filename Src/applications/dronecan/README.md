The node has the following registers:

| №  | Register name           | Description |
| -- | ----------------------- | ----------- |
|  1 | uavcan.node.id          | Defines a node-ID. Allowed values [0,127]. |
|  2 | system.name             | Defines custom node name. If empty, the node will use the default name. |
|  3 | system.protocol         | Auto, Cyphal/CAN or DroneCAN |
|  4 | pwm.cmd_ttl_ms          | TTL of specified by pwm.cmd_type commands [ms]. |
|  5 | pwm.frequency           | PWM frequency [Hz]. |
|  6 | pwm.cmd_type            | 0 means RawCommand, 1 means ArrayCommand, 2 is reserved for hardpoint.Command. |
|  7 | pwm.1_ch                | Index of setpoint channel. [-1; 255]. -1 means disabled, |
|  8 | pwm.1_min               | PWM duration when setpoint is min (RawCommand is 0 or Command is -1.0) |
|  9 | pwm.1_max               | PWM duration when setpoint is max (RawCommand is 8191 or Command is 1.0) |
|  10 | pwm.1_def               | PWM duration when setpoint is negative or there is no setpoint at all. |
|  11 | pwm.1_feedback          | Indicates the operational mode of the node. 0 means disabled. When set to 1, the command of corresponding Status type for cmd_type will be transmitted (esc.RawCommand - esc.Status, actuator.ArrayCommand - actuator.Status) with frequency 1 Hz. When set to 2 - 10 Hz. |
|  12 | pwm.2_ch                | Index of setpoint channel. [-1; 255]. -1 means disabled, |
|  13 | pwm.2_min               | PWM duration when setpoint is min (RawCommand is 0 or Command is -1.0) |
|  14 | pwm.2_max               | PWM duration when setpoint is max (RawCommand is 8191 or Command is 1.0) |
|  15 | pwm.2_def               | PWM duration when setpoint is negative or there is no setpoint at all. |
|  16 | pwm.2_feedback          | Indicates the operational mode of the node. 0 means disabled. When set to 1, the command of corresponding Status type for cmd_type will be transmitted (esc.RawCommand - esc.Status, actuator.ArrayCommand - actuator.Status) with frequency 1 Hz. When set to 2 - 10 Hz. |
|  17 | pwm.3_ch                | Index of setpoint channel. [-1; 255]. -1 means disabled, |
|  18 | pwm.3_min               | PWM duration when setpoint is min (RawCommand is 0 or Command is -1.0) |
|  19 | pwm.3_max               | PWM duration when setpoint is max (RawCommand is 8191 or Command is 1.0) |
|  20 | pwm.3_def               | PWM duration when setpoint is negative or there is no setpoint at all. |
|  21 | pwm.3_feedback          | Indicates the operational mode of the node. 0 means disabled. When set to 1, the command of corresponding Status type for cmd_type will be transmitted (esc.RawCommand - esc.Status, actuator.ArrayCommand - actuator.Status) with frequency 1 Hz. When set to 2 - 10 Hz. |
|  22 | pwm.4_ch                | Index of setpoint channel. [-1; 255]. -1 means disabled, |
|  23 | pwm.4_min               | PWM duration when setpoint is min (RawCommand is 0 or Command is -1.0) |
|  24 | pwm.4_max               | PWM duration when setpoint is max (RawCommand is 8191 or Command is 1.0) |
|  25 | pwm.4_def               | PWM duration when setpoint is negative or there is no setpoint at all. |
|  26 | pwm.4_feedback          | Indicates the operational mode of the node. 0 means disabled. When set to 1, the command of corresponding Status type for cmd_type will be transmitted (esc.RawCommand - esc.Status, actuator.ArrayCommand - actuator.Status) with frequency 1 Hz. When set to 2 - 10 Hz. |
|  27 | imu.enable              | Enable or disable IMU publisher. |

> This docs was automatically generated. Do not edit it manually.

