The node has the following interface:

| №  | Type | Message | Topic name  |
| -- | ---- | ------- | ----------- |
|   1 | pub | uavcan.si.sample.voltage.Scalar.1.0 | crct.5v | {'type': 'Port', 'note': 'Voltage 5V', 'data_type': 'uavcan.si.sample.voltage.Scalar.1.0', 'enum_base': 'PARAM_PUB_CRCT_5V'}|
|   2 | pub | uavcan.si.sample.voltage.Scalar.1.0 | crct.vin | {'type': 'Port', 'note': 'Voltage Vin', 'data_type': 'uavcan.si.sample.voltage.Scalar.1.0', 'enum_base': 'PARAM_PUB_CRCT_VIN'}|
|   3 | pub | uavcan.si.sample.temperature.Scalar.1.0 | crct.temperature | {'type': 'Port', 'note': 'STM32 internal temperature', 'data_type': 'uavcan.si.sample.temperature.Scalar.1.0', 'enum_base': 'PARAM_PUB_CRCT_TEMPERATURE'}|
|   4 | sub | reg.udral.service.actuator.common.sp.Vector31 | setpoint | {'type': 'Port', 'data_type': 'reg.udral.service.actuator.common.sp.Vector31', 'enum_base': 'PARAM_SUB_SETPOINT'}|
|   5 | pub | reg.udral.service.actuator.common.Feedback.0.1 | feedback | {'type': 'Port', 'data_type': 'reg.udral.service.actuator.common.Feedback.0.1', 'enum_base': 'PARAM_PUB_FEEDBACK_1'}|

The node has the following registers:

| №  | Register name           | Description |
| -- | ----------------------- | ----------- |
|  1 | uavcan.node.id          | Defines a node-ID. Allowed values [0,127]. |
|  2 | system.name             | Defines custom node name. If empty, the node will use the default name. |
|  3 | uavcan.node.description | User/integrator-defined, human-readable description of this specific node. |
|  4 | example.integer         | Just an integer register example |
|  5 | example.string          | Just a string register example |

> This docs was automatically generated. Do not edit it manually.

