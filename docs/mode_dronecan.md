# DroneCAN backend

The backend lives in [Src/modules/backends/dronecan](../Src/modules/backends/dronecan):
`core` runs the node (NodeStatus, GetNodeInfo, parameters), and each topic has a
frontend next to it. Copy one to add your own:

| Example | What it shows |
|---|---|
| [backends/dronecan/board_status](../Src/modules/backends/dronecan/board_status) | A publisher of a datapool topic, with its parameters in `params.yaml` |
| [rcout/dronecan_frontend](../Src/modules/rcout/dronecan_frontend) | Subscribers that drive the PWM outputs |
| [feedback/dronecan](../Src/modules/feedback/dronecan) | Publishers of actuator feedback |

Try it on SITL or on a node behind a CAN sniffer:

```bash
make rl_sitl_dronecan run         # or: make rl_mini_v2_dronecan upload
dronecan_gui_tool
```

The [Mini node docs](https://docs.raccoonlab.co/guide/can_pwm/can_pwm_dronecan.html)
walk through a bench test with gui_tool.
