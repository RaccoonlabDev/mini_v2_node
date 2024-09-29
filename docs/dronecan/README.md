The node has the following registers:

| Register name           | Description |
| ----------------------- | ----------- |
| uavcan.node.id          | Defines a node-ID. Allowed values [0,127]. |
| uavcan.node.description | User/integrator-defined, human-readable description of this specific node. |
| system.can_terminator   | Bitmask which switch can terminators (0 - both off, 1 - enables second, 2 - enables first, 3 - enables both). Works only for v3. |
| system.name             | Defines custom node name. If empty, the node will use the default name. |
| system.protocol         | Auto, Cyphal/CAN or DroneCAN |
| system.log_level        | Log level. See [debug.LogLevel](https://dronecan.github.io/Specification/7._List_of_standard_data_types/#loglevel) and [diagnostic.Severity](https://github.com/OpenCyphal/public_regulated_data_types/blob/master/uavcan/diagnostic/Severity.1.0.dsdl). </br> 0 - Log everything (DEBUG, INFO, WARNING, ERROR) </br> 1 - Log at least INFO level </br> 2 - Log at least WARNING level </br> 3 - Log at least ERROR level </br> 4 - Disable logging </br> By default 3 to show only realy important messages. |
| stats.engaged_time      | Total engaged time of node in seconds. |
| crct.bitmask            | Bit mask to enable CircuitStatus features: </br> Bit 1 - enable dev.Temperature with device_id=NODE_ID, </br> Bit 2 - enable 5v publisher, </br> Bit 3 - enable Vin publisher. </br> Bit 4 - enable overvoltage, undervoltage, overcurrent and overheat checks. </br> By default 15 that mean enable all publishers |
| feedback.type           | Indicates the operational mode of the node. 0 means disabled. When set to 1, the command of corresponding Status type for cmd_type will be transmitted (esc.RawCommand - esc.Status, actuator.ArrayCommand - actuator.Status) with frequency 1 Hz. When set to 2 - 10 Hz. |
| pwm.cmd_ttl_ms          | TTL of specified by pwm.cmd_type commands [ms]. |
| pwm.frequency           | PWM frequency [Hz]. |
| pwm1.ch                 | Index of setpoint channel. [-1; 255]. -1 means disabled, |
| pwm1.min                | PWM duration when setpoint is min (RawCommand is 0 or Command is -1.0) |
| pwm1.max                | PWM duration when setpoint is max (RawCommand is 8191 or Command is 1.0) |
| pwm1.def                | PWM duration when setpoint is negative or there is no setpoint at all. |
| pwm2.ch                 | Index of setpoint channel. [-1; 255]. -1 means disabled, |
| pwm2.min                | PWM duration when setpoint is min (RawCommand is 0 or Command is -1.0) |
| pwm2.max                | PWM duration when setpoint is max (RawCommand is 8191 or Command is 1.0) |
| pwm2.def                | PWM duration when setpoint is negative or there is no setpoint at all. |
| pwm3.ch                 | Index of setpoint channel. [-1; 255]. -1 means disabled, |
| pwm3.min                | PWM duration when setpoint is min (RawCommand is 0 or Command is -1.0) |
| pwm3.max                | PWM duration when setpoint is max (RawCommand is 8191 or Command is 1.0) |
| pwm3.def                | PWM duration when setpoint is negative or there is no setpoint at all. |
| pwm4.ch                 | Index of setpoint channel. [-1; 255]. -1 means disabled, |
| pwm4.min                | PWM duration when setpoint is min (RawCommand is 0 or Command is -1.0) |
| pwm4.max                | PWM duration when setpoint is max (RawCommand is 8191 or Command is 1.0) |
| pwm4.def                | PWM duration when setpoint is negative or there is no setpoint at all. |
| pwm.cmd_type            | 0 means RawCommand, 1 means ArrayCommand, 2 is reserved for hardpoint.Command. |
| imu.enable              | Enable or disable IMU publisher. |

> This docs was automatically generated. Do not edit it manually.

