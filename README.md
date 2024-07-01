[![Code Smells](https://sonarcloud.io/api/project_badges/measure?project=RaccoonlabDev_mini_v2_node&metric=code_smells)](https://sonarcloud.io/summary/new_code?id=RaccoonlabDev_mini_v2_node) [![Lines of Code](https://sonarcloud.io/api/project_badges/measure?project=RaccoonlabDev_mini_v2_node&metric=ncloc)](https://sonarcloud.io/summary/new_code?id=RaccoonlabDev_mini_v2_node) [![cyphal](https://github.com/RaccoonlabDev/mini_v2_node/actions/workflows/cyphal.yml/badge.svg)](https://github.com/RaccoonlabDev/mini_v2_node/actions/workflows/cyphal.yml) [![dronecan](https://github.com/RaccoonlabDev/mini_v2_node/actions/workflows/dronecan.yml/badge.svg)](https://github.com/RaccoonlabDev/mini_v2_node/actions/workflows/dronecan.yml) [![dronecan](https://github.com/RaccoonlabDev/mini_v2_node/actions/workflows/code_style.yml/badge.svg)](https://github.com/RaccoonlabDev/mini_v2_node/actions/workflows/code_style.yml)

# Mini v2 Cyphal/DroneCAN application template

This repo introduces a simple ready-to-use Cyphal/DroneCAN application template for [RL Mini v2](https://docs.raccoonlab.co/guide/can_pwm/can_pwm_mini_v2.html) and [RL Mini v3](https://docs.raccoonlab.co/guide/can_pwm/mini_v3.html) nodes.

Please, refer Wiki for details:
- [Cyphal template application](https://github.com/RaccoonlabDev/mini_v2_node/wiki/Cyphal-application),
- [DroneCAN template application](https://github.com/RaccoonlabDev/mini_v2_node/wiki/DroneCAN-application).

### 1. Key features

- Minimal recommended Cyphal features are supported out of the box,
- You can run the software in SITL mode (don't need to have any hardware),
- Cyphal firmware binary is only 35.2 KB.

Not suported yet / In Roadmap:
- Both Cyphal/CAN and DroneCAN protocols in a single firmware
- Bootloader
- Dynamics Node Allocation (DNA)
- MPU-9250 IMU (node v3)
- Redundant CAN interface (node v3)
- CAN terminator resistors control (node v3)

### 2. Supported boards

The software supports RL v2 nodes (stm32f103, 128 KBytes flash) and v3 nodes (stm32g0b1, 512 KBytes flash).

| Name | View | Pinout | STM32CubeMX |
| ---- | ---- | ------ | ----------- |
| Mini v2 | <img src="https://docs.raccoonlab.co/assets/img/view_top.6b0ef99e.png" alt="drawing" width="150"> | <img src="https://docs.raccoonlab.co/assets/img/pinout.c14a3021.png" alt="drawing" width="150"> | <img src="https://raw.githubusercontent.com/RaccoonLabHardware/mini_v2_stm32cubemx_project/main/Assets/stm32cubemx.png" alt="drawing" width="200"> |
| Mini v3 | <img src="https://docs.raccoonlab.co/assets/img/t-view-bottom.7eadba26.png" alt="drawing" width="150"> | <img src="https://docs.raccoonlab.co/assets/img/pinout.e7b1d6b7.png" alt="drawing" width="150"> | <img src="https://github.com/RaccoonLabHardware/v3-software-template/blob/main/Assets/stm32cubemx.png" alt="drawing" width="200"> |

### 3. Brief Cyphal/DroneCAN capabilities

You can build the software either for Cyphal/CAN or DroneCAN. By default both applications are as simple as possible. They:
- subscribe on setpoint topics and control PWM1-4 channels,
- measure 5v and Vin voltage, temperature, current (if supported) and publish as circuit status and actuator feedback,
- blink the RGB LED to indicate the current state.

In the table below you can see interfaces of the applications.

| Cyphal/CAN | DroneCAN |
| ---------- | -------- |
| Subscribe on: </br> - setpoint ([reg.udral.service.actuator.common.sp.Vector31](https://github.com/OpenCyphal/public_regulated_data_types/blob/master/reg/udral/service/actuator/common/sp/Vector31.0.1.dsdl)) </br> </br> </br> Publish: </br> - feedback ([reg.udral.service.actuator.common.Feedback](https://github.com/OpenCyphal/public_regulated_data_types/blob/master/reg/udral/service/actuator/common/Feedback.0.1.dsdl)) </br> - crct.5v ([uavcan.si.unit.voltage.Scalar](https://github.com/OpenCyphal/public_regulated_data_types/blob/master/uavcan/si/unit/voltage/Scalar.1.0.dsdl)) </br> - crct.vin ([uavcan.si.unit.voltage.Scalar](https://github.com/OpenCyphal/public_regulated_data_types/blob/master/uavcan/si/unit/voltage/Scalar.1.0.dsdl)) </br> - crct.temp ([uavcan.si.unit.temperature.Scalar](https://github.com/OpenCyphal/public_regulated_data_types/blob/master/uavcan/si/unit/temperature/Scalar.1.0.dsdl)) </br> . | Subscribe on: </br> - [uavcan.equipment.esc.RawCommand](https://dronecan.github.io/Specification/7._List_of_standard_data_types/#rawcommand) </br> - [uavcan.equipment.actuator.ArrayComman](https://dronecan.github.io/Specification/7._List_of_standard_data_types/#arraycommand) </br> - [uavcan.equipment.hardpoint.Command](https://dronecan.github.io/Specification/7._List_of_standard_data_types/#command-1) </br> Publish: </br > - [uavcan.equipment.esc.Status](https://dronecan.github.io/Specification/7._List_of_standard_data_types/#status-2) </br > - [uavcan.equipment.actuator.Status](https://dronecan.github.io/Specification/7._List_of_standard_data_types/#status) </br > - [uavcan.equipment.hardpoint.Status](https://dronecan.github.io/Specification/7._List_of_standard_data_types/#status-3) </br > - [uavcan.equipment.power.CircuitStatus](https://dronecan.github.io/Specification/7._List_of_standard_data_types/#circuitstatus) </br> - [uavcan.equipment.device.Temperature](https://dronecan.github.io/Specification/7._List_of_standard_data_types/#temperature) |
| <img src="https://raw.githubusercontent.com/wiki/RaccoonlabDev/mini_v2_node/assets/yukon.png" alt="drawing" width=450> | <img src="https://raw.githubusercontent.com/wiki/RaccoonlabDev/mini_v2_node/assets/dronecan_interface.png" alt="drawing" width=152> <img src="https://raw.githubusercontent.com/wiki/RaccoonlabDev/mini_v2_node/assets/dronecan_params.png" alt="drawing" width=147> |

### 4. Use cases

1. Control 1-4 motors without feedback or servos with current feedback (if hardware supports)

    <img src="https://docs.raccoonlab.co/assets/img/mini_v2_with_servo.4761fdf2.png" alt="drawing" width="400">

2. Control 1-2 motors with UART/ADC feedback

    <img src="https://docs.raccoonlab.co/assets/img/mini_v2_with_esc_flame.0ffb552f.png" alt="drawing" width="400">

3. Cyphal/DroneCAN adapter for I2C/UART sensor

    <img src="https://docs.raccoonlab.co/assets/img/lw20_i2c.82bad2a4.png" alt="drawing" width="400">

4. Control a servo with current feedback (if hardware supports) and angular position feedback (from [AS5600 I2C sensor](https://docs.raccoonlab.co/guide/as5600/))
5. Control a PWM cargo gripper

### 5. Q&A

If you are strugguling with the software building, please refer to the build workflow [build.yml](.github/workflows/build.yml) for a hint. If it doesn't help, you can open [an issue]( https://github.com/RaccoonlabDev/mini_v2_node/issues?q=is%3Aissue+).

### 6. More examples

Consider the following projects based on this template examples:

1. [RL UAV Lights node](https://github.com/RaccoonlabDev/uav_lights_node/tree/lights)
2. [IU VTOL PMU node](https://github.com/Innopolis-UAV-Team/vtol_pmu_node)
3. [IU Airspeed+rangefinder+ligths+PWM node](https://github.com/Innopolis-UAV-Team/lights_node)
4. [IU Fuel tank node](https://github.com/Innopolis-UAV-Team/fuel_tank_node)
