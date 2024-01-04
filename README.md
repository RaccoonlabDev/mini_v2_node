# Mini v2 node Cyphal/DroneCAN application template

This repository introduces a simple Cyphal/DroneCAN application written for the [RL Mini v2](https://docs.raccoonlab.co/guide/can_pwm/can_pwm_mini_v2.html) and [RL Micro](https://docs.raccoonlab.co/guide/can_pwm/can_pwm_micro.html) nodes. Based on this project you can write your custom application if the original CAN-PWM convertor application doesn't suit your requirements.

| View | Top view | Bot view |
| ---- | --- | ------ |
| <img src="https://docs.raccoonlab.co/assets/img/view.bbf1e631.png" alt="drawing" width="150"> | <img src="https://docs.raccoonlab.co/assets/img/view_top.6b0ef99e.png" alt="drawing" width="150"> | <img src="https://docs.raccoonlab.co/assets/img/view_bottom.ee41f6d5.png" alt="drawing" width="150">|

The default capabilities of the node are shown on the picture below:

<img src="assets/yukon.png" alt="drawing">

## 1. Prerequisites

### 1.1. Hardware requirements

- [Mini v2 or Micro node](https://docs.raccoonlab.co/guide/can_pwm/),
- STM32 programmer and CAN-sniffer (for example [RL sniffer and programmer](https://docs.raccoonlab.co/guide/programmer_sniffer/)).

> Actually the repository can be suitable for any RaccoonLab nodes with HW version v2 (it basically means a stm32f103 board with a few specific design rules), but it is primarily intended for RaccoonLab Mini v2 node.

### 1.2. Software requirements

You are expected to use the following software:
- (optional) [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html) or [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html),
- Cyphal: [Yakut CLI](https://github.com/OpenCyphal/yakut) is recommended and [Yukon GUI](https://github.com/OpenCyphal-Garage/yukon) is optional.

## 2. Design notes

**2.1. Submodules**

The project has a few dependencies which are attached to the repository as submodules. They are:

- [Libs/stm32-cube-project](https://github.com/RaccoonLabHardware/mini_v2_ioc) is a project generated with the STM32CubeMX. It is based on .ioc file corresponded to the default firmware of the Mini v2 node. You may only need to change it if you want to use an a different peripheral configuration.
- [Libs/Cyphal](https://github.com/RaccoonlabDev/libcanard_cyphal_application) is a general-purpose application based on the [Cyphal libcanard](https://github.com/OpenCyphal/libcanard), [o1heap](https://github.com/pavel-kirienko/o1heap) and other libraries with minimal required features to start and some features related to UDRAL/DS015.
- [Src/libparams](https://github.com/PonomarevDA/libparams) is a simple library with ROM driver implementation that allows to store configuration parameters in persistent memory.
- [not yet] Src/libsqcan is a general-purpose application based on [DroneCAN libcanard](https://github.com/dronecan/libcanard) with minimal required features to start. The DroneCAN part is not publically released yet.

**2.2. Makefile usage**

The project is based on the CMake build system, but it is suggested to interract with [Makefile](Makefile). This is just a wrapper under CMake, useful for its target autocompletion.

To build a binary, you need to type either `make cyphal` or `make dronecan` based on which protocol you want to use.

To upload a binary, you need to type `make upload`.

You can build a binary and upload it using a single command: `make cyphal upload`. 

The result binary will be built in `build` folder.

**2.3. Pre-build step**

Before actual build process, it generates a few additional source files into `build/src` such as:
- parameters based on yaml files,
- software version and git hash based on current commit.

**2.4. STM32 Periphery**

The entry point of the program is main() function of [Libs/stm32-cube-project/Core/Src/main.c](https://github.com/RaccoonLabHardware/mini-v2-software/blob/main/Core/Src/main.c). This as well as all other peripheral initialization is pre-generated based on the configuratation file [project.ioc](https://github.com/RaccoonLabHardware/mini-v2-software/blob/main/can_pwm_v2.ioc). If you want to use a different peripheral configuration, you should update project.ioc with STM32CubeIDE or STM32CubeMX.

**2.5. Application entry point**

After the peripheral initialization the application goes to the application_entry_point() in [Src/cyphal_application/application.cpp](Src/cyphal_application/application.cpp). It is assumed that a user will provide a custom application here.

**2.6. Default application**

By default the application just blinks the RGB LED, subscribes to the setpoint topic to control PWM1 and publishes a feedback with the latest applied setpoint. Note, that the application is as simple as possible: it controls only a single PWM and doesn't have safety features like TTL, but you are free to extend it as you want.

## 3. Writing a custom application

> The following steps assume that you are already successful with the default application provided in this repository and want to customize it.

**Step 1. Make a template or fork of the repository. Then clone it. Don't forget about the submodules.**

```bash
git clone https://github.com/RaccoonlabDev/mini_v2_node --recursive
cd mini_v2_node
git submodule update --init --recursive
```

**(Optionally) Step 2. Set the desired periphery config**

Mini v2 node has 6 user's pins. By default 4 of them are configured as PWM and 2 of them as UART RX. The pinout configuration is shown on the image below:

<img src="https://docs.raccoonlab.co/assets/img/pinout.c14a3021.png" alt="drawing">

If you want to use a custom periphery configuration, update the [Libs/stm32-cube-project/project.ioc](Libs/stm32-cube-project/project.ioc) file with STM32CubeIDE or STM32CubeMX and regenerate the project.

The default pinout configuration is shown below:

<img src="https://raw.githubusercontent.com/RaccoonLabHardware/mini_v2_stm32cubemx_project/main/Assets/stm32cubemx.png" alt="drawing">

Since [Libs/stm32-cube-project](Libs/stm32-cube-project) is submodule, you can either use your own submodule (make a fork or template [RaccoonLabHardware/mini-v2-software](https://github.com/RaccoonLabHardware/mini-v2-software)) or just remove the submodule and include the source code in the root repository.

**(Optionally) Step 3. Modify the default application**

The main application is started in [Src/cyphal_application/application.cpp](Src/cyphal_application/application.cpp).
By default it just blinks the RGB LED, subscribes to the setpoint topic to control PWM1 and publishes a feedback with the latest applied setpoint.

> Note, that the application is as simple as possible: it controls only a single PWM and doesn't have safety features like TTL, but you are free to extend it as you want.

**(Optionally) Step 4. Add custom Cyphal-topics**

You may consider [Src/cyphal_application/setpoint](Src/cyphal_application/setpoint) and [Src/cyphal_application/feedback](Src/cyphal_application/feedback) as examples of Cyphal-subscriber and Cyphal-publisher. If you create subjects by inhereting from CyphalPublisher and CyphalSubscriber, it will automatically add all topics to the port.List array, so the node is able to advertise its capabilities. This approach doesn't automatically create registers, so you need to add them in yaml file as shown in the examples.

**(Optionally) Step 5. Add custom registers**

Each Cyphal topics requires a pair of Integer (for port id) and String (for port type) registers.

> This section is in progress. Please, check examples or libparams for the details.

When you add your custom module, don't forget to add source file and path to the file with registers to [Src/cyphal_application/CMakeLists.txt](Src/cyphal_application/CMakeLists.txt). 

You can also easily create custom Integer and String registers. An example is shown in [Src/cyphal_application/params.yaml](Src/cyphal_application/params.yaml).

**Step 6. Connect Sniffer and Programmer to Mini v2 node.**

An example of connection scheme suitable for bench test for Mini v2 node and RL Programmer-Sniffer is shown below:

<img src="assets/connection.png" alt="drawing">

You can also use other sniffer and programmers. For details refer to: [Programmer usage](https://docs.raccoonlab.co/guide/programmer_sniffer/programmer.html) and [Sniffer usage](https://docs.raccoonlab.co/guide/programmer_sniffer/sniffer.html#_4-1-cyphal-usage) pages.

**Step 7. Build the project and upload the firmware**

```bash
make generate_dsdl  # you need to call it only before the first build
make cyphal
make upload         # it works only with RaccoonLab sniffer-programmer yet
```

As a short form, you can build and upload the firmware with a single command:

```bash
make cyphal upload
```

A few details about how the build process works:
- `make generate_dsdl` calls the script to generate C++ headers for Cyphal data types serialization using nunavut. The output goes into [build/compile_output](build/compile_output) and [build/nunavut_out](build/nunavut_out) folders. It is expected that you doesn't often change DSDL, so you typically need to call it only once. 
- `make cyphal` before the actual build process generates a few files as well. Specifically, it generates:
    - [build/src/git_software_version.h](build/src/git_software_version.h) that has info about the software version based on latest git tag,
    - [build/src/git_hash.h](build/src/git_hash.h) with info about the current commit,
    - C++ source and header files with parameters array and enums based on all associated yaml files with registers (you can find generated files in the same folder: [build/src](build/src)),
    - [Src/cyphal_application/README.md](Src/cyphal_application/README.md) with info about the supported interface and not port-related registers.

**Step 8. Setup the environment and run Yukon**

Try the command below or use the official yukon/yakut instructions.

```bash
source scripts/init.sh
~/Download/Yukon
```

Please, refer to the [Mini node docs](https://docs.raccoonlab.co/guide/can_pwm/can_pwm_cyphal.html).
It has a detailed steps about how to perform bench testing of the node.

**Q&A**

If you are strugguling with the software building, please refer to the build workflow [.github/workflows/build.yml](.github/workflows/build.yml) for a hint. If it doesn't help, you can open an issue.
