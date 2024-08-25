[![Code Smells](https://sonarcloud.io/api/project_badges/measure?project=RaccoonlabDev_mini_v2_node&metric=code_smells)](https://sonarcloud.io/summary/new_code?id=RaccoonlabDev_mini_v2_node) [![Lines of Code](https://sonarcloud.io/api/project_badges/measure?project=RaccoonlabDev_mini_v2_node&metric=ncloc)](https://sonarcloud.io/summary/new_code?id=RaccoonlabDev_mini_v2_node) [![cyphal](https://github.com/RaccoonlabDev/mini_v2_node/actions/workflows/cyphal.yml/badge.svg)](https://github.com/RaccoonlabDev/mini_v2_node/actions/workflows/cyphal.yml) [![dronecan](https://github.com/RaccoonlabDev/mini_v2_node/actions/workflows/dronecan.yml/badge.svg)](https://github.com/RaccoonlabDev/mini_v2_node/actions/workflows/dronecan.yml) [![dronecan](https://github.com/RaccoonlabDev/mini_v2_node/actions/workflows/code_style.yml/badge.svg)](https://github.com/RaccoonlabDev/mini_v2_node/actions/workflows/code_style.yml)

# Cyphal/DroneCAN application

This repo introduces a ready-to-use Cyphal/DroneCAN application for [RL Mini v2](https://docs.raccoonlab.co/guide/can_pwm/can_pwm_mini_v2.html) and [RL Mini v3](https://docs.raccoonlab.co/guide/can_pwm/mini_v3.html) nodes.

Please, refer Wiki for details:
- [Cyphal template application](https://github.com/RaccoonlabDev/mini_v2_node/wiki/Cyphal-application),
- [DroneCAN template application](https://github.com/RaccoonlabDev/mini_v2_node/wiki/DroneCAN-application).

### 1. KEY FEATURES

- [x] Both Cyphal/CAN and DroneCAN protocols or one of them in a single firmware,
- [x] You can try the software in SITL mode (don't need to have any hardware),
- [x] Small firmware image and software is friendly for customization,
- [x] Redundant CAN interface (node v3),
- [x] MPU-9250 IMU (node v3).

Not suported or tested yet / In Roadmap:
- [ ] Cyphal/DroneCAN Bootloader,
- [ ] Dynamics Node Allocation (DNA),
- [ ] Vibration Analysis: estimate dominant frequency and magnitude of vibrations for diagnosing mechanical issues or ensuring smooth operation in drones or robotic platforms in real time,
- [ ] CAN terminator resistors control (node v3).

### 2. SUPPORTED HARDWARE

The software supports RL v2 nodes (stm32f103, 128 KBytes flash) and v3 nodes (stm32g0b1, 512 KBytes flash).

| Name | View | Pinout | STM32CubeMX |
| ---- | ---- | ------ | ----------- |
| Mini v2 </br> stm32f103 </br> 128 KBytes flash | <img src="https://docs.raccoonlab.co/assets/img/view_top.6b0ef99e.png" alt="drawing" width="120"> | <img src="https://docs.raccoonlab.co/assets/img/pinout.c14a3021.png" alt="drawing" width="120"> | <img src="https://raw.githubusercontent.com/RaccoonLabHardware/mini_v2_stm32cubemx_project/main/Assets/stm32cubemx.png" alt="drawing" width="160"> |
| Mini v3 </br> stm32fg0b1 </br> 512 KBytes flash | <img src="https://docs.raccoonlab.co/assets/img/t-view-bottom.7eadba26.png" alt="drawing" width="120"> | <img src="https://docs.raccoonlab.co/assets/img/pinout.e7b1d6b7.png" alt="drawing" width="120"> | <img src="https://github.com/RaccoonLabHardware/v3-software-template/blob/main/Assets/stm32cubemx.png" alt="drawing" width="160"> |

### 3. USE CASES

This node is a versatile device integrating PWM control, IMU sensing, and dual CAN bus communication, suitable for various applications in robotics, drones, and other embedded systems. Below are the detailed use cases for different scenarios.

**1. Basic ESC and Servo Control**

- Use PWM1-4 to control ESC via Cyphal or DroneCAN esc.RawCommand or actuator.ArrayCommand messages.
- Connect standard 5V servos to PWM1-PWM2 for controlling servo motors. Ideal for use cases requiring precise angular movement, such as robotic arms or UAV control surfaces.

    <img src="https://docs.raccoonlab.co/assets/img/mini_v2_with_servo.4761fdf2.png" alt="drawing" width="400">

**2. Advanced ESC Control with Feedback**

- Utilize PWM3 or PWM4 for controlling ESCs with UART or ADC feedback on the FB pin.

    <img src="https://docs.raccoonlab.co/assets/img/mini_v2_with_esc_flame.0ffb552f.png" alt="drawing" width="400">

**3. CAN-I2C Converter**

- Configure PWM1 and PWM2 as I2C pins to act as a CAN-I2C converter.
- Use the node as an adapter to connect I2C peripherals like rangefinders or environmental sensors to a CAN-based network.

    <img src="https://docs.raccoonlab.co/assets/img/lw20_i2c.82bad2a4.png" alt="drawing" width="400">

**4. Cargo Gripper**

- Use PWM1 or PWM2 to control servo-based cargo grippers via DroneCAN hardpoint.Command messages.
- Ideal for UAV payload management systems, enabling secure and precise handling of cargo.

**5. Dual CAN Bus Interface for Protocol Conversion**

- Leverage the node’s dual CAN interfaces (CAN1 and CAN2) to convert between different CAN protocols.
- For example, connect CAN1 to an autopilot using DroneCAN or Cyphal/CAN, and CAN2 to a motor or servo using canopen or another protocol.
- This enables seamless integration between devices using different CAN standards, making it ideal for systems where components from various manufacturers need to communicate.

    > Supported only on Mini v3 node

**6. Servo Position Estimation with I2C Encoder**
- Connect a servo motor to PWM3 or PWM4 and an I2C encoder to PWM1/SDA and PWM2/SDC to estimate the angular position of the servo.
- Consider [AS5600 I2C sensor](https://docs.raccoonlab.co/guide/as5600/) encoder.

    > An example of this feature is not included in the repository yet...

**7. Vibration Analysis with IMU**

- Utilize the onboard IMU (MPU9250) to monitor vibrations in your system.
- Estimate dominant frequency and magnitude of vibrations, which is crucial for diagnosing mechanical issues or ensuring smooth operation in drones or robotic platforms.

    > Software doesn't support it yet...


### 4. Q&A

If you are strugguling with the software building, please refer to the build workflow [build.yml](.github/workflows/build.yml) for a hint. If it doesn't help, you can open [an issue]( https://github.com/RaccoonlabDev/mini_v2_node/issues?q=is%3Aissue+).

### 5. More examples

Consider the following projects as examples:

1. [RL UAV Lights node](https://github.com/RaccoonlabDev/uav_lights_node/tree/lights)
2. [IU VTOL PMU node](https://github.com/Innopolis-UAV-Team/vtol_pmu_node)
3. [IU Airspeed+rangefinder+ligths+PWM node](https://github.com/Innopolis-UAV-Team/lights_node)
4. [IU Fuel tank node](https://github.com/Innopolis-UAV-Team/fuel_tank_node)
