[![Code Smells](https://sonarcloud.io/api/project_badges/measure?project=RaccoonlabDev_mini_v2_node&metric=code_smells)](https://sonarcloud.io/summary/new_code?id=RaccoonlabDev_mini_v2_node) [![LOC](https://sonarcloud.io/api/project_badges/measure?project=RaccoonlabDev_mini_v2_node&metric=ncloc)](https://sonarcloud.io/summary/new_code?id=RaccoonlabDev_mini_v2_node) [![Cyphal SITL](https://github.com/RaccoonlabDev/mini_v2_node/actions/workflows/cyphal_sitl.yml/badge.svg)](https://github.com/RaccoonlabDev/mini_v2_node/actions/workflows/cyphal_sitl.yml) [![DroneCAN SITL](https://github.com/RaccoonlabDev/mini_v2_node/actions/workflows/dronecan_sitl.yml/badge.svg)](https://github.com/RaccoonlabDev/mini_v2_node/actions/workflows/dronecan_sitl.yml) [![Code Style](https://github.com/RaccoonlabDev/mini_v2_node/actions/workflows/code_style.yml/badge.svg)](https://github.com/RaccoonlabDev/mini_v2_node/actions/workflows/code_style.yml) [![Build & Deploy](https://github.com/RaccoonlabDev/mini_v2_node/actions/workflows/build_and_deploy.yml/badge.svg)](https://github.com/RaccoonlabDev/mini_v2_node/actions/workflows/build_and_deploy.yml)

# Cyphal/DroneCAN application

This repo introduces a ready-to-use Cyphal/DroneCAN application for [RL Mini v2](https://docs.raccoonlab.co/guide/can_pwm/can_pwm_mini_v2.html) and [RL Mini v3](https://docs.raccoonlab.co/guide/can_pwm/mini_v3.html) nodes.

Please, refer Wiki for details:
- [Cyphal template application](https://github.com/RaccoonlabDev/mini_v2_node/wiki/Cyphal-application),
- [DroneCAN template application](https://github.com/RaccoonlabDev/mini_v2_node/wiki/DroneCAN-application).
Additional app docs:
- [Gimbal application](docs/gimbal.md).

### 1. KEY FEATURES

Supported features:
- [x] Both Cyphal/CAN and DroneCAN protocols or one of them in a single firmware,
- [x] You can try the software in Software-In-The-Loop (SITL) mode,
- [x] Small firmware image and software is friendly for customization,
- [x] Redundant CAN interface (node v3),
- [x] MPU-9250 IMU (node v3).
- [x] CAN terminator resistors control (node v3).

Roadmap / Not supported or tested yet:
- [ ] Cyphal/DroneCAN Bootloader,
- [ ] Dynamic Node Allocation (DNA),
- [ ] Vibration Analysis: estimate dominant frequency and magnitude of vibrations for diagnosing mechanical issues or ensuring smooth operation in drones or robotic platforms in real time.
- [ ] AS5600 I2C sensor encoder for servo position estimation

### 2. TARGET HARDWARE

The software supports RL v2 nodes (stm32f103, 128 KBytes flash) and v3 nodes (stm32g0b1, 512 KBytes flash).

| Name | View | Pinout | STM32CubeMX |
| ---- | ---- | ------ | ----------- |
| Mini v2 </br> stm32f103 </br> 128 KBytes flash | <img src="https://docs.raccoonlab.co/assets/img/view_top.6b0ef99e.png" alt="drawing" width="120"> | <img src="https://docs.raccoonlab.co/assets/img/pinout.c14a3021.png" alt="drawing" width="120"> | <img src="https://raw.githubusercontent.com/RaccoonLabHardware/mini_v2_stm32cubemx_project/main/Assets/stm32cubemx.png" alt="drawing" width="160"> |
| Mini v3 </br> stm32g0b1 </br> 512 KBytes flash | <img src="https://docs.raccoonlab.co/assets/img/t-view-bottom.7eadba26.png" alt="drawing" width="120"> | <img src="https://docs.raccoonlab.co/assets/img/pinout.e7b1d6b7.png" alt="drawing" width="120"> | <img src="https://github.com/RaccoonLabHardware/v3-software-template/blob/main/Assets/stm32cubemx.png" alt="drawing" width="160"> |

### 3. USE CASES

This node is a versatile device integrating PWM control, IMU sensing, and dual CAN bus communication, suitable for various applications in robotics, drones, and other embedded systems. Below are the detailed use cases for different scenarios.

| Use case | Description |
|-|-|
| <img src="https://docs.raccoonlab.co/assets/img/mini_v2_with_servo.4761fdf2.png" alt="drawing" width="400"> | **1. Basic ESC and Servo Control** </br> PWM1-4 to control PWM ESCs via Cyphal/DroneCAN. </br> PWM1-2 to control 5V servos. </br> DroneCAN interface: [RawCommand](https://dronecan.github.io/Specification/7._List_of_standard_data_types/#rawcommand) / [ArrayCommand](https://dronecan.github.io/Specification/7._List_of_standard_data_types/#arraycommand). </br> Cyphal/CAN interface: [UDRAL setpoint](https://github.com/OpenCyphal/public_regulated_data_types/blob/master/reg/udral/service/actuator/esc/_.0.1.dsdl) |
| <img src="https://docs.raccoonlab.co/assets/img/mini_v2_with_esc_flame.0ffb552f.png" alt="drawing" width="400"> | **2. Advanced ESC Control with Feedback** </br> PWM3-4 for controlling PWM ESCs </br> UART or ADC feedback on the FB pin. </br> DroneCAN interface: [esc.Status](https://dronecan.github.io/Specification/7._List_of_standard_data_types/#status-2) / [actuator.Status](https://dronecan.github.io/Specification/7._List_of_standard_data_types/#status). </br> Cyphal/CAN interface: [UDRAL Feedback](https://github.com/OpenCyphal/public_regulated_data_types/blob/master/reg/udral/service/actuator/common/Feedback.0.1.dsdl) |
| <img src="https://docs.raccoonlab.co/assets/img/lw20_i2c.82bad2a4.png" alt="drawing" width="400"> | **3. CAN-I2C Converter** </br> PWM1 and PWM2 as I2C pins. </br> Cyphal/DroneCAN adapter for I2C peripherals. </br> DroneCAN examples: [range_sensor.Measurement](https://dronecan.github.io/Specification/7._List_of_standard_data_types/#measurement).|
| | **4. Cargo Gripper** </br> PWM1-2 to control servo-based cargo grippers. </br> UAV payload management systems. </br> DroneCAN interface: [hardpoint.Command](https://dronecan.github.io/Specification/7._List_of_standard_data_types/#command-1). |
| | **5. CAN-CAN converter** </br> CAN1 for Cyphal/DroneCAN. </br> CAN2 for another CAN protocol. </br> Cyphal/DroneCAN adapter for another CAN device. </br> Examples: KST/CAN servo, CANopen motor, CAN BMS. </br> Keep a consistent CAN network. </br> Supported only on Mini v3 node. |
| | **6. Servo Position Estimation with I2C Encoder** </br> PWM3-4 for a servo </br> PWM1-2 for a [AS5600 I2C encoder](https://docs.raccoonlab.co/guide/as5600/). </br> Estimate the angular position of the servo. </br> DroneCAN interface: [actuator.Status](https://dronecan.github.io/Specification/7._List_of_standard_data_types/#status). |
| | **7. Vibration Analysis with IMU** </br> Utilize the onboard IMU (MPU-9250) </br> Monitor vibrations in your system. </br> Estimate dominant frequency and magnitude of vibrations. </br> Crucial for diagnosing mechanical issues </br> or ensuring smooth operation in drones or robotic platforms. </br> DroneCAN interface: [ahrs.RawIMU](https://dronecan.github.io/Specification/7._List_of_standard_data_types/#rawimu). |

### 4. SOFTWARE PREREQUISITES

The following table outlines the compatibility of the project with different operating systems, modes, and support timelines.

| **Operating System** | **DroneCAN Support** | **Cyphal Support** | **Notes**                              |
|----------------------|----------------------|--------------------|----------------------------------------|
| Manjaro (latest)     | ✅ Supported         | ✅ Supported       | Latest Manjaro latest version.            |
| Ubuntu 24.04         | ✅ Supported         | ✅ Supported       | Latest Ubuntu LTS version.             |
| Ubuntu 22.04         | ✅ Supported         | ✅ Supported       | Fully supported for both modes.        |
| Ubuntu 20.04         | ❌ Not Supported     | ❌ Not Supported   | Deprecated in 2025-04-15.              |
| Windows (2022)       | ✅ Supported         | ✅ Supported       | Current "latest" Windows version.      |

Requirements:
- arm-none-eabi-gcc
- CMake
- Python
- STM32CubeMX/STM32CubeIDE
- st-link

### 5. QUICK START

**Installation** (Ubuntu, Manjaro, Windows):

```bash
# 1. Make a fork of the repository and clone the repository with submodules
git clone git@github.com:RaccoonlabDev/mini_v2_node.git --recursive

# 2. Install python requirements
pip install -r requirements.txt

# 3. Install other requirements (toolchains, etc.)
./scripts/install.py
```

**Build** (examples):

```bash
# DroneCAN firmware:
make dronecan_v2
make dronecan_v3

# Cyphal firmware:
make cyphal_v2
make cyphal_v3
```

**Upload/Flash**:

```bash
make dronecan_v2 upload
# or:
make dronecan_v3 upload
```

**SITL (Software-In-The-Loop)**:

```bash
make sitl_dronecan run
# or:
make sitl_cyphal run
```

**GUI Tools**

For full interaction with the node use:
- DroneCAN: [gui_tool](https://github.com/dronecan/gui_tool)
- Cyphal: [Yakut](https://github.com/OpenCyphal/yakut) / [Yukon](https://github.com/OpenCyphal/yukon).

### 6. ARCHITECTURE OVERVIEW

The project is organized into three main layers to make it portable across boards:

1. **Peripheral Layer ([Src/peripheral](Src/peripheral))**
    - STM32CubeMX-generated HAL code is wrapped in lightweight C++ classes.
    - High-level code accesses peripherals only through the `HAL` namespace.
    - Goal: minimize direct dependency on STM32CubeMX and ease migration to other platforms.
2. **Board Support / Platform Layer ([Src/platform](Src/platform))**
    - Board-specific configuration (pins, ADC channels, LEDs, CAN terminators, etc.).
3. **High-Level Layer**
    - **Drivers ([Src/drivers](Src/drivers))** — external devices (e.g. IMU).
    - **Modules ([Src/modules](Src/modules))** — application logic, e.g. vibration metrics.
    - Uses DroneCAN or Cyphal/CAN for communication.

**Build Targets**
- Build and flash using Make, for example: `make dronecan_v3`
- STM32CubeMX is used only to generate peripheral code. Application logic starts in `application_entry_point()` called from `main.c`.

**Standards & Hardware Info**
- Boards follow an internal standard for peripherals (ADC_VIN, ADC_5V, ADC_CURRENT, RGB LED, CAN_TERMINATORS, etc.).
See [docs/hardware.md](docs/hardware.md) for hardware details.

### 7. Basic Application Logic & LED Status

All nodes run a minimal application after flashing so you can verify the board is working without extra setup:
- **CAN communication** starts according to the selected build target (DroneCAN or Cyphal/CAN).
- **LED indicator** follows a standard pattern common to all Raccoonlab boards (similar to [PX4 LED Meanings](https://docs.px4.io/main/en/getting_started/led_meanings.html) and [Ardupilot LED meanings](https://ardupilot.org/copter/docs/common-leds-pixhawk.html)):

| LED Pattern & Color | Meaning |
|-|-|
| **Blinking Blue/Red** | Initialization / Calibration       |
| **Blinking Yellow**   | Minor Failure / Warning            |
| **Blinking Magenta**  | Major Failure / Error              |
| **Blinking Red**      | Fatal Malfunction / Critical       |
| **Blinking Blue**     | Ready — No GPS Lock                |
| **Solid Blue**¹       | Armed — No GPS Lock                |
| **Blinking Green**²   | Ready — 3D Fix (8+ satellites)     |
| **Solid Green**¹²     | Armed — 3D Fix (8+ satellites)     |

¹ Applicable to nodes that support an **armed** state (e.g. actuator nodes).

² Applicable to nodes that use **GPS** (e.g. GNSS or combined nodes).

### 8. CREATING YOUR OWN APPLICATION

This repository can be used in two ways:

| Approach | When to Use | Summary |
| -------- | ----------- | ------- |
| **Fork (recommended)**  | Use it by default | One fork per user/org |
| **Template (advanced)** | When multiple apps are needed. | Unlimited repos per user/org, but extra steps to restore history and no automatic pull requests. |

<details open> <summary><b>▶ Workflow 1 — Fork (Recommended)</b></summary>

1. Fork this repository on GitHub (top-right “Fork” button).
2. Clone your fork:
    ```bash
    git clone git@github.com:<your_org>/mini_v2_node.git --recursive
    ```
3. Create your feature branch from the main branch:
    ```bash
    git checkout -b pr-my-feature origin/main
    ```
4. Push and open pull requests as usual:
    ```bash
    git push origin pr-my-feature
    ```

    ✅ This is the simplest and most familiar workflow.

</details>

<details> <summary><b>▶ Workflow 2 — Template (Advanced, for Multiple Apps)</b></summary>

1. Create a new repository from this template
    Click “Use this template” on GitHub → create your own repo.
2. Restore the full commit history with tags from the original repo
    ```bash
    git remote add upstream https://github.com/RaccoonlabDev/mini_v2_node.git
    git fetch --tags upstream
    ```
3. Create your feature branch from the latest available tag
    ```bash
    git checkout -b custom-<LATEST_TAG> <LATEST_TAG> # branch named after upstream version
    git push origin custom-<LATEST_TAG>
    ```
4. **Optional**: Add a readme branch for extended documentation or onboarding if you want to keep docs separate from code.
    ```bash
    git checkout --orphan readme
    git rm -rf .
    echo "hello there" > README.md
    git add README.md
    git commit -m "first commit"
    git push origin readme
    ```

    ✅ This workflow is ideal if you need multiple independent applications, separate permissions per repo, and still want to keep upstream history.

</details>

**Custom application versioning**
- Recommended format: `v<upstream-version>-<app-version>`
- Example: `v1.3.0-0.1.0` (based on upstream `v1.3.0`, app version `0.1.0`).

### 9. Q&A

If you are struggling with the software building, please refer to the build workflow [build_and_deploy.yml](.github/workflows/build_and_deploy.yml) for a hint. If it doesn't help, you can open [an issue]( https://github.com/RaccoonlabDev/mini_v2_node/issues?q=is%3Aissue+).

### 10. MORE EXAMPLES

Consider the following projects as examples:

1. [RL UAV Lights node](https://github.com/RaccoonlabDev/uav_lights_node/tree/lights)
2. [IU VTOL PMU node](https://github.com/Innopolis-UAV-Team/vtol_pmu_node)
