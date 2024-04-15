The node has the following registers:

| №  | Register name           | Description |
| -- | ----------------------- | ----------- |
|  1 | uavcan.node.id          | Defines a node-ID. Allowed values [0,127]. |
|  2 | system.name             | Defines custom node name. If empty, the node will use the default name. |
|  3 | pwm.cmd_ttl_ms          | TTL of LightCommands [ms]. |
|  4 | pwm.frequency           | PWM frequency [Hz]. |
|  5 | pwm.cmd_type            | 0 means RawCommand, 1 means ArrayCommand, 2 is reserved for hardpoint.Command. |
|  6 | pwm.1_ch                | Index of setpoint channel. [-1; 255]. -1 means disabled, |
|  7 | pwm.1_min               | PWM duration when setpoint is min (RawCommand is 0 or Command is 0.0) |
|  8 | pwm.1_max               | PWM duration when setpoint is max (RawCommand is 8191 or Command is 1.0) |
|  9 | pwm.1_def               | PWM duration when setpoint is negative or there is no setpoint at all. |
|  10 | pwm.2_ch                | Index of setpoint channel. [-1; 255]. -1 means disabled, |
|  11 | pwm.2_min               | PWM duration when setpoint is min (RawCommand is 0 or Command is 0.0) |
|  12 | pwm.2_max               | PWM duration when setpoint is max (RawCommand is 8191 or Command is 1.0) |
|  13 | pwm.2_def               | PWM duration when setpoint is negative or there is no setpoint at all. |
|  14 | pwm.3_ch                | Index of setpoint channel. [-1; 255]. -1 means disabled, |
|  15 | pwm.3_min               | PWM duration when setpoint is min (RawCommand is 0 or Command is 0.0) |
|  16 | pwm.3_max               | PWM duration when setpoint is max (RawCommand is 8191 or Command is 1.0) |
|  17 | pwm.3_def               | PWM duration when setpoint is negative or there is no setpoint at all. |
|  18 | pwm.4_ch                | Index of setpoint channel. [-1; 255]. -1 means disabled, |
|  19 | pwm.4_min               | PWM duration when setpoint is min (RawCommand is 0 or Command is 0.0) |
|  20 | pwm.4_max               | PWM duration when setpoint is max (RawCommand is 8191 or Command is 1.0) |
|  21 | pwm.4_def               | PWM duration when setpoint is negative or there is no setpoint at all. |
|  22 | pwm.verbose             | Indicates the operational mode of the node. When set to 1, the command of type cmd_type will be transmitted. |

> This docs was automatically generated. Do not edit it manually.

