[![Code Smells](https://sonarcloud.io/api/project_badges/measure?project=RaccoonlabDev_mini_v2_node&metric=code_smells)](https://sonarcloud.io/summary/new_code?id=RaccoonlabDev_mini_v2_node) [![Lines of Code](https://sonarcloud.io/api/project_badges/measure?project=RaccoonlabDev_mini_v2_node&metric=ncloc)](https://sonarcloud.io/summary/new_code?id=RaccoonlabDev_mini_v2_node) [![cyphal](https://github.com/RaccoonlabDev/mini_v2_node/actions/workflows/cyphal.yml/badge.svg)](https://github.com/RaccoonlabDev/mini_v2_node/actions/workflows/cyphal.yml) [![dronecan](https://github.com/RaccoonlabDev/mini_v2_node/actions/workflows/dronecan.yml/badge.svg)](https://github.com/RaccoonlabDev/mini_v2_node/actions/workflows/dronecan.yml) [![dronecan](https://github.com/RaccoonlabDev/mini_v2_node/actions/workflows/code_style.yml/badge.svg)](https://github.com/RaccoonlabDev/mini_v2_node/actions/workflows/code_style.yml)

# Mini v2 Cyphal/DroneCAN application template

This repo introduces a simple ready-to-use Cyphal/DroneCAN application template for [RL Mini v2](https://docs.raccoonlab.co/guide/can_pwm/can_pwm_mini_v2.html) node.

Please, refer Wiki for details:
- [Cyphal template application](https://github.com/RaccoonlabDev/mini_v2_node/wiki/Cyphal-application),
- [DroneCAN template application](https://github.com/RaccoonlabDev/mini_v2_node/wiki/DroneCAN-application).

### 1. Key features

- Minimal recommended Cyphal features are supported out of the box (bootloader and DNA are coming next),
- You can run the software in SITL mode (don't need to have any hardware),
- Cyphal firmware binary is only 35.2 KB.

### 2. Brief hardware info

| Top view | Bottom view | Pinout | STM32CubeMX |
| --- | ------ |-|-|
| <img src="https://docs.raccoonlab.co/assets/img/view_top.6b0ef99e.png" alt="drawing" width="200"> | <img src="https://docs.raccoonlab.co/assets/img/view_bottom.ee41f6d5.png" alt="drawing" width="200"> | <img src="https://docs.raccoonlab.co/assets/img/pinout.c14a3021.png" alt="drawing" width="200"> | <img src="https://raw.githubusercontent.com/RaccoonLabHardware/mini_v2_stm32cubemx_project/main/Assets/stm32cubemx.png" alt="drawing" width="200"> |

### 3. Brief Cyphal capabilities

By default the application is as simple as possible. It just blinks the RGB LED, subscribes to the setpoint topic to control a single PWM (PWM1) and publishes a feedback with the latest applied setpoint. It doesn't have any safety features like TTL, but you are free to extend it as you want.

The default capabilities of the node are shown on the picture below:

<img src="https://raw.githubusercontent.com/wiki/RaccoonlabDev/mini_v2_node/assets/yukon.png" alt="drawing">

### 4. Brief DroneCAN capabilities

The application subscribes on a message configured in `pwm.cmd_type`. It supports: [esc.RawCommand](https://dronecan.github.io/Specification/7._List_of_standard_data_types/#rawcommand), [actuator.ArrayCommand](https://dronecan.github.io/Specification/7._List_of_standard_data_types/#arraycommand), [hardpoint.Command](https://dronecan.github.io/Specification/7._List_of_standard_data_types/#command-1). It controls the PWM and sends back [esc.Status](https://dronecan.github.io/Specification/7._List_of_standard_data_types/#status-2), [actuator.Status](https://dronecan.github.io/Specification/7._List_of_standard_data_types/#status), [hardpoint.Status](https://dronecan.github.io/Specification/7._List_of_standard_data_types/#status-3) as feedback. It also sends [power.CircuitStatus](https://dronecan.github.io/Specification/7._List_of_standard_data_types/#circuitstatus) with the circuit state (5v voltage, vin voltage, current) and [device.Temperature](https://dronecan.github.io/Specification/7._List_of_standard_data_types/#temperature) with stm32 internal measured temperature.

|||
|-|-|
| <img src="https://raw.githubusercontent.com/wiki/RaccoonlabDev/mini_v2_node/assets/dronecan_interface.png" alt="drawing" width=305> | <img src="https://raw.githubusercontent.com/wiki/RaccoonlabDev/mini_v2_node/assets/dronecan_params.png" alt="drawing" width=295> |

### 5. Q&A

If you are strugguling with the software building, please refer to the build workflow [build.yml](.github/workflows/build.yml) for a hint. If it doesn't help, you can open [an issue]( https://github.com/RaccoonlabDev/mini_v2_node/issues?q=is%3Aissue+).

### 6. More examples

Consider the following projects based on this template examples:

1. [RL UAV Lights node](https://github.com/RaccoonlabDev/uav_lights_node/tree/lights)
2. [IU VTOL PMU node](https://github.com/Innopolis-UAV-Team/vtol_pmu_node)
3. [IU Airspeed+rangefinder+ligths+PWM node](https://github.com/Innopolis-UAV-Team/lights_node)
4. [IU Fuel tank node](https://github.com/Innopolis-UAV-Team/fuel_tank_node)
