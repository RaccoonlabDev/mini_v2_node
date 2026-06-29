# Supported CAN backends

At the CAN layer the firmware is protocol-agnostic — in principle any CAN protocol can be implemented. Backends currently provided, selectable per build target (`NC_TARGET`):

| Backend | Build target | Docs |
|-|-|-|
| DroneCAN | `dronecan` | [mode_dronecan.md](mode_dronecan.md) |
| Cyphal/CAN | `cyphal` | [mode_cyphal.md](mode_cyphal.md) |
| Both at once | `both` | [both.md](both.md) |

Also available:
- A **CANopen** module (runs under the DroneCAN backend).
- Bridging to other CAN protocols (e.g. CANopen motor, CAN BMS) — see the *CAN-CAN converter* use case in the README.
