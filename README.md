# Mini v2 node custom application

This repository introduces a simple Cyphal application written for the [RL Mini v2](https://docs.raccoonlab.co/guide/can_pwm/can_pwm_mini_v2.html) and [RL Micro](https://docs.raccoonlab.co/guide/can_pwm/can_pwm_micro.html) nodes. Based on this project you can write your custom application if the original CAN-PWM convertor application doesn't suit your requirements.

| View | Top view | Bot view |
| ---- | --- | ------ |
| <img src="https://docs.raccoonlab.co/assets/img/view.bbf1e631.png" alt="drawing" width="150"> | <img src="https://docs.raccoonlab.co/assets/img/view_top.6b0ef99e.png" alt="drawing" width="150"> | <img src="https://docs.raccoonlab.co/assets/img/view_bottom.ee41f6d5.png" alt="drawing" width="150">|

The default capabilities of the node are shown on the picture below:

<img src="assets/yukon.png" alt="drawing">

## 1. Brief hardware description

The node has 6 user pins. By default 4 of them are configured as PWM and 2 of them as UART RX, but you can change the configuration to support I2C, ADC, GPIO or something else.

Please, refer to the [Mini v2 hardware](https://docs.raccoonlab.co/guide/can_pwm/can_pwm_mini_v2.html#pinout) page for the details.

Below you can see a brief description of hardware capabilities:

<img src="https://docs.raccoonlab.co/assets/img/pinout.c14a3021.png" alt="drawing">

## 2. STM32CubeMX

The pinout configuration is based on the [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html) generated project: [Libs/mini_v2](https://github.com/RaccoonLabHardware/mini_v2_ioc) with the following configuration:

<img src="https://raw.githubusercontent.com/RaccoonLabHardware/mini_v2_stm32cubemx_project/main/Assets/stm32cubemx.png" alt="drawing">

If you need to use custom pinout configuration, it is recommended to use either [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html) or [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html) to modify .ioc file and regenerate your custom project.

## 3. Prerequisites

You are expected to use the following software:
- (optional) [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html) or [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html),
- [Yukon GUI](https://github.com/OpenCyphal-Garage/yukon) and [Yakut CLI](https://github.com/OpenCyphal/yakut) for Cyphal,
- [gui_tool](https://dronecan.github.io/GUI_Tool/Overview/) for DroneCAN.

Hardware requirements:
- Mini v2 or Micro node
- STM32 programmer and CAN-sniffer (for example [RL sniffer and programmer](https://docs.raccoonlab.co/guide/programmer_sniffer/))

## 4. Usage

The project is based on CMake build system, but it is proposed to use [Makefile](Makefile) as some kind of wrapper under CMake.

**Step 1. Clone the repository with submodules**

```bash
git clone https://github.com/RaccoonlabDev/mini_v2_node --recursive
cd mini_v2_node
git submodule update --init --recursive
```

**Step 2. Connect Sniffer and Programmer to Mini v2 node.**

An example of connection scheme for Mini v2 node and RL Programmer-Sniffer is shown below:

<img src="assets/connection.png" alt="drawing">

For details refer to: [Programmer usage](https://docs.raccoonlab.co/guide/programmer_sniffer/programmer.html) and [Sniffer usage](https://docs.raccoonlab.co/guide/programmer_sniffer/sniffer.html#_4-1-cyphal-usage) pages.

**Step 3. Build the project and upload the firmware**

```bash
make generate_dsdl # you need to call it only before the first build
make cyphal
make upload
```

**Step 4. Setup the environment and run Yukon**

```bash
source scripts/init.sh
~/Download/Yukon
```

**Q&A**

If you are strugguling with the software building, please refer to the build workflow [.github/workflows/build.yml](.github/workflows/build.yml) for a hint. If it doesn't help, you can open an issue.

## 5. Customization

The peripherals are initialised in [Libs/mini-v2-software/Core/Src/main.c](Libs/mini-v2-software/Core/Src/main.c), which are automatically generated based on the configuratation file [can_pwm_v2.ioc](Libs/mini-v2-software/can_pwm_v2.ioc) file. If you want to use a different peripheral configuration, you should update can_pwm_v2.ioc with STM32CubeIDE or STM32CubeMX.

The main application is started in [Src/cyphal_application/application.cpp](Src/cyphal_application/application.cpp).
By default it just blinks the RGB LED, subscribes to the setpoint topic, controls a single PWM and publishes a feedback to the feedback topic. Note, that the application is as simple as possible and doesn't have safety features like TTL.

You may consider [Src/cyphal_application/setpoint](Src/cyphal_application/setpoint) and [Src/cyphal_application/feedback](Src/cyphal_application/feedback) as examples of Cyphal-subscriber and Cyphal-publisher. It is recommended to create subjects by inhereting from CyphalPublisher or CyphalSubscriber and creating required registers in yaml file. This approach automatically adds all topics to the port.List array, allowing the node is able to advertise its capabilities.

When you add your custom module, don't forget to add source file and path to the file with registers to [Src/cyphal_application/CMakeLists.txt](Src/cyphal_application/CMakeLists.txt). 

You can also easily create custom Integer and String registers. An example is shown in [Src/cyphal_application/params.yaml](Src/cyphal_application/params.yaml).

## 6. Reference

The project has a few dependencies which are attached to the repository as submodules. They are:

- [Libs/mini_v2](https://github.com/RaccoonLabHardware/mini_v2_ioc) is a project generated with the STM32CubeMX. It is based on .ioc file corresponded to the default firmware of the Mini v2 node. You may only need to change it if you want to use an a different peripheral configuration.
- [Libs/Cyphal](https://github.com/RaccoonlabDev/libcanard_cyphal_application) is a general-purpose application based on the [Cyphal libcanard](https://github.com/OpenCyphal/libcanard), [o1heap](https://github.com/pavel-kirienko/o1heap) and other libraries with minimal required features to start and some features related to UDRAL/DS015.
- [Src/libparams](https://github.com/PonomarevDA/libparams) is a simple library with ROM driver implementation that allows to store configuration parameters in persistent memory.
- [not yet] Src/libsqcan is a general-purpose application based on [DroneCAN libcanard](https://github.com/dronecan/libcanard) with minimal required features to start. The DroneCAN part is not publically released yet.
