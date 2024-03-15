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
|  2 | system.name             | Defines custom node name. If empty, the node will use the default name. |
|  3 | uavcan.node.description | User/integrator-defined, human-readable description of this specific node. |
|  4 | example.integer         | Just an integer register example |
|  5 | example.string          | Just a string register example |

> This docs was automatically generated. Do not edit it manually.

