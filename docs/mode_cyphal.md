# Cyphal/CAN backend

The backend lives in [Src/modules/backends/cyphal](../Src/modules/backends/cyphal):
`core` runs the node on the private `libcpnode` (heartbeat, GetInfo, port list,
registers), and each topic has a frontend next to it. Copy one to add your own:

| Example | What it shows |
|---|---|
| [backends/cyphal/board_status](../Src/modules/backends/cyphal/board_status) | Publishers of a datapool topic, with their port registers in `params.yaml` |
| [rcout/cyphal_frontend](../Src/modules/rcout/cyphal_frontend) | A subscriber that drives the PWM outputs |
| [feedback/cyphal](../Src/modules/feedback/cyphal) | Publishers of actuator feedback |

A publisher or subscriber is advertised in `port.List` on its own, and is off
until its `uavcan.pub.*.id` / `uavcan.sub.*.id` register gets a port id. Try it
on SITL or on a node behind a CAN sniffer with [Yakut](https://github.com/OpenCyphal/yakut):

```bash
make rl_sitl_cyphal run           # or: make rl_mini_v2_cyphal upload
y mon                             # see who is on the bus
y r 42 uavcan.pub.crct.temperature.id 1000
y sub 1000:uavcan.si.sample.temperature.scalar
```

[Yukon](https://github.com/OpenCyphal/yukon) shows the same in a GUI. The
[Mini node docs](https://docs.raccoonlab.co/guide/can_pwm/can_pwm_cyphal.html)
walk through a bench test.
