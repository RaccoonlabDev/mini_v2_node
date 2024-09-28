The node has the following interface:

Cyphal Publishers:
| №  | Data type and topic name  | Description |
| -- | ------------------------- | ----------- |
|  1 | [uavcan.si.sample.voltage.Scalar](https://github.com/OpenCyphal/public_regulated_data_types/blob/master/uavcan/si/sample/voltage/Scalar.1.0.dsdl) </br> crct.5v | Voltage 5V (after DC-DC).</br>The normal values are within [4.5, 5.5] Volts.</br>|
|  2 | [uavcan.si.sample.voltage.Scalar](https://github.com/OpenCyphal/public_regulated_data_types/blob/master/uavcan/si/sample/voltage/Scalar.1.0.dsdl) </br> crct.vin | Voltage Vin (before DC-DC).</br>It make sense only when you power up the node from Molex 6-pin connector.</br>The normal values are within [5.5, 55] Volts.</br>|
|  3 | [uavcan.si.sample.temperature.Scalar](https://github.com/OpenCyphal/public_regulated_data_types/blob/master/uavcan/si/sample/temperature/Scalar.1.0.dsdl) </br> crct.temperature | STM32 internal temperature.</br>It it expected that the temperature is a little bit higher then your room environment.</br>|
|  4 | [reg.udral.service.actuator.common.Feedback](https://github.com/OpenCyphal/public_regulated_data_types/blob/master/reg/udral/service/actuator/common/Feedback.0.1.dsdl) </br> feedback | |

Cyphal Subscribers:
| №  | Data type and topic name  | Description |
| -- | ------------------------- | ----------- |
|  1 | [reg.udral.service.actuator.common.sp.Vector31](https://github.com/OpenCyphal/public_regulated_data_types/blob/master/reg/udral/service/actuator/common/sp/Vector31.0.1.dsdl) </br> setpoint | |

The node has the following registers:

| №  | Register name           | Description |
| -- | ----------------------- | ----------- |
|  1 | uavcan.node.id          | Defines a node-ID. Allowed values [0,127]. |
|  2 | system.can_terminator   | Bitmask which switch can terminators (0 - both off, 1 - enables second, 2 - enables first, 3 - enables both). Works only for v3. |
|  3 | system.name             | Defines custom node name. If empty, the node will use the default name. |
|  4 | system.protocol         | Auto, Cyphal/CAN or DroneCAN |
|  5 | uavcan.node.description | User/integrator-defined, human-readable description of this specific node. |
|  6 | system.log_level        | Log level. See [debug.LogLevel](https://dronecan.github.io/Specification/7._List_of_standard_data_types/#loglevel) and [diagnostic.Severity](https://github.com/OpenCyphal/public_regulated_data_types/blob/master/uavcan/diagnostic/Severity.1.0.dsdl). </br> 0 - Log everything (DEBUG, INFO, WARNING, ERROR) </br> 1 - Log at least INFO level </br> 2 - Log at least WARNING level </br> 3 - Log at least ERROR level </br> 4 - Disable logging </br> By default 3 to show only realy important messages. |
|  7 | pwm.cmd_ttl_ms          | TTL of specified by pwm.cmd_type commands [ms]. |
|  8 | pwm.frequency           | PWM frequency [Hz]. |
|  9 | pwm1.ch                 | Index of setpoint channel. [-1; 255]. -1 means disabled, |
|  10 | pwm1.min                | PWM duration when setpoint is min (RawCommand is 0 or Command is -1.0) |
|  11 | pwm1.max                | PWM duration when setpoint is max (RawCommand is 8191 or Command is 1.0) |
|  12 | pwm1.def                | PWM duration when setpoint is negative or there is no setpoint at all. |
|  13 | pwm2.ch                 | Index of setpoint channel. [-1; 255]. -1 means disabled, |
|  14 | pwm2.min                | PWM duration when setpoint is min (RawCommand is 0 or Command is -1.0) |
|  15 | pwm2.max                | PWM duration when setpoint is max (RawCommand is 8191 or Command is 1.0) |
|  16 | pwm2.def                | PWM duration when setpoint is negative or there is no setpoint at all. |
|  17 | pwm3.ch                 | Index of setpoint channel. [-1; 255]. -1 means disabled, |
|  18 | pwm3.min                | PWM duration when setpoint is min (RawCommand is 0 or Command is -1.0) |
|  19 | pwm3.max                | PWM duration when setpoint is max (RawCommand is 8191 or Command is 1.0) |
|  20 | pwm3.def                | PWM duration when setpoint is negative or there is no setpoint at all. |
|  21 | pwm4.ch                 | Index of setpoint channel. [-1; 255]. -1 means disabled, |
|  22 | pwm4.min                | PWM duration when setpoint is min (RawCommand is 0 or Command is -1.0) |
|  23 | pwm4.max                | PWM duration when setpoint is max (RawCommand is 8191 or Command is 1.0) |
|  24 | pwm4.def                | PWM duration when setpoint is negative or there is no setpoint at all. |

> This docs was automatically generated. Do not edit it manually.

