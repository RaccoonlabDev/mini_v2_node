The node has the following interface:

Cyphal Publishers:
| Data type and topic name  | Description |
| ------------------------- | ----------- |
| [uavcan.si.sample.voltage.Scalar](https://github.com/OpenCyphal/public_regulated_data_types/blob/master/uavcan/si/sample/voltage/Scalar.1.0.dsdl) </br> crct.5v | Voltage 5V (after DC-DC).</br>The normal values are within [4.5, 5.5] Volts.</br>|
| [uavcan.si.sample.voltage.Scalar](https://github.com/OpenCyphal/public_regulated_data_types/blob/master/uavcan/si/sample/voltage/Scalar.1.0.dsdl) </br> crct.vin | Voltage Vin (before DC-DC).</br>It make sense only when you power up the node from Molex 6-pin connector.</br>The normal values are within [5.5, 55] Volts.</br>|
| [uavcan.si.sample.temperature.Scalar](https://github.com/OpenCyphal/public_regulated_data_types/blob/master/uavcan/si/sample/temperature/Scalar.1.0.dsdl) </br> crct.temperature | STM32 internal temperature.</br>It it expected that the temperature is a little bit higher then your room environment.</br>|
| [reg.udral.service.actuator.common.Feedback](https://github.com/OpenCyphal/public_regulated_data_types/blob/master/reg/udral/service/actuator/common/Feedback.0.1.dsdl) </br> udral_feedback1 | |
| [reg.udral.service.actuator.common.Feedback](https://github.com/OpenCyphal/public_regulated_data_types/blob/master/reg/udral/service/actuator/common/Feedback.0.1.dsdl) </br> udral_feedback2 | |
| [reg.udral.service.actuator.common.Feedback](https://github.com/OpenCyphal/public_regulated_data_types/blob/master/reg/udral/service/actuator/common/Feedback.0.1.dsdl) </br> udral_feedback3 | |
| [reg.udral.service.actuator.common.Feedback](https://github.com/OpenCyphal/public_regulated_data_types/blob/master/reg/udral/service/actuator/common/Feedback.0.1.dsdl) </br> udral_feedback4 | |
| [zubax.telega.CompactFeedback.0.1](zubax.telega.CompactFeedback.0.1) </br> zubax_feedback1 | |
| [zubax.telega.CompactFeedback.0.1](zubax.telega.CompactFeedback.0.1) </br> zubax_feedback2 | |
| [zubax.telega.CompactFeedback.0.1](zubax.telega.CompactFeedback.0.1) </br> zubax_feedback3 | |
| [zubax.telega.CompactFeedback.0.1](zubax.telega.CompactFeedback.0.1) </br> zubax_feedback4 | |

Cyphal Subscribers:
| Data type and topic name  | Description |
| ------------------------- | ----------- |
| [reg.udral.service.actuator.common.sp.Vector31](https://github.com/OpenCyphal/public_regulated_data_types/blob/master/reg/udral/service/actuator/common/sp/Vector31.0.1.dsdl) </br> setpoint | |

The node has the following registers:

| Register name           | Description |
| ----------------------- | ----------- |
| uavcan.node.id          | Defines a node-ID. Allowed values [0,127]. |
| uavcan.node.description | User/integrator-defined, human-readable description of this specific node. |
| system.log_level        | Log level. See [debug.LogLevel](https://dronecan.github.io/Specification/7._List_of_standard_data_types/#loglevel) and [diagnostic.Severity](https://github.com/OpenCyphal/public_regulated_data_types/blob/master/uavcan/diagnostic/Severity.1.0.dsdl). </br> 0 - Log everything (DEBUG, INFO, WARNING, ERROR) </br> 1 - Log at least INFO level </br> 2 - Log at least WARNING level </br> 3 - Log at least ERROR level </br> 4 - Disable logging </br> By default 3 to show only realy important messages. |
| system.name             | Defines custom node name. If empty, the node will use the default name. |
| system.reserved1        | Reservew for parameters crc calculation |
| system.can_terminator   | Bitmask which switch can terminators (0 - both off, 1 - enables second, 2 - enables first, 3 - enables both). Works only for v3. |
| system.protocol         | Auto, Cyphal/CAN or DroneCAN |
| system.reserved4        | Reserved |
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
| stats.engaged_time      | Total engaged time of node in seconds. |
| crct.bitmask            | Bit mask to enable CircuitStatus features: </br> Bit 1 - enable dev.Temperature with device_id=NODE_ID, </br> Bit 2 - enable 5v publisher, </br> Bit 3 - enable Vin publisher. </br> Bit 4 - enable overvoltage, undervoltage, overcurrent and overheat checks. </br> By default 15 that mean enable all publishers |
| feedback.esc.enable     | 0 - disabled, 1 - enabled |
| feedback.actuator.enable | 0 - disabled, 1 - enabled |
| feedback.hardpoint.enable | 0 - disabled, 1 - enabled |
| imu.mode                | Bit mask to enable IMU features: </br> Bit 1 - enable RawImu publisher, </br> Bit 2 - enable vibration metric publishing to RawImu.integration_interval, </br> Bit 3 - enable FFT acceleration publishing to RawImu.accelerometer_integral, </br> Bit 4 - enable FFT acceleration publishing to RawImu.rate_gyro_integral, </br> By default 15 that mean enable all publishers |
| imu.pub_frequency       | Frequency of IMU publisher [Hz]. |

> This docs was automatically generated. Do not edit it manually.

