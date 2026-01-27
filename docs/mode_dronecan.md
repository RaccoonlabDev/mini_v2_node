# DroneCAN application

## 1. Hardware requirements

The following hardware are required:

| № | Required device | Example | Image |
|-|-|-|-|
| 1 | Target device | [Mini v2 or Micro node](https://docs.raccoonlab.co/guide/can_pwm/) | <img src="https://docs.raccoonlab.co/assets/img/view_top.6b0ef99e.png" alt="drawing" width="150"> |
| 2 | Programmer, CAN-sniffer | [RL sniffer and programmer](https://docs.raccoonlab.co/guide/programmer_sniffer/) | <img src="https://docs.raccoonlab.co/assets/img/view-top.504299a8.png" alt="drawing" width="150"> |

> Actually the repository can be suitable for any RaccoonLab nodes with HW version v2 (it basically means a stm32f103 board with a few specific design rules), but it is primarily intended for RaccoonLab Mini v2 node.

## 2. Software requirements

You are expected to use the following software:
- stm32 related: cmake, arm-none-eabi-gcc, stlink,
- (optional) [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html) or [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html),
- [gui_tool](https://dronecan.github.io/GUI_Tool/Overview/).

## 3. Design notes

### 3.1. Structure and dependencies

The project depends on a few libraries which are attached to the repository as submodules. The dependency graph can be illustrated as shown below:

<img src="https://raw.githubusercontent.com/wiki/RaccoonlabDev/mini_v2_node/assets/dronecan.svg" alt="drawing">

A few notes:
- [Libs/stm32-cube-project](https://github.com/RaccoonLabHardware/mini_v2_ioc) is a project generated with the STM32CubeMX. It is based on .ioc file corresponded to the default firmware of the Mini v2 node. You may only need to change it if you want to use an a different peripheral configuration.
- [Src/libparams](https://github.com/PonomarevDA/libparams) is a simple library with ROM driver implementation that allows to store configuration parameters in persistent memory.
- [Libs/Dronecan](Libs/Dronecan) provides DroneCAN/libcanard sources and serialization to build a minimal DroneCAN application.

### 3.2. Makefile

The project is based on the CMake build system, but it is suggested to interract with [Makefile](Makefile). This is just a wrapper under CMake, useful for its autocompletion.

Check [Makefile](Makefile) for additional commands and details.

The main commads are following:

**1. Build an exacutable and run in SITL mode**

```bash
make sitl_dronecan run
```

**2. Build and upload a binary**

To build a binary and upload it to the target, you can type:

```bash
make dronecan upload
```

The result binary will be built in automatically created `build` folder.

### 3.3. Pre-build step

When you run `make dronecan` before actual build process, a few additional source files are automatically generated into `build/src` such as:
- [build/src/git_software_version.h](build/src/git_software_version.h) that has info about the software version based on latest git tag,
- [build/src/git_hash.h](build/src/git_hash.h) with info about the current commit,
- [params.cpp](build/src/params.cpp) and [params.hpp](build/src/params.hpp) - C++ source and header files with parameters array and enums based on all associated yaml files with registers (you can find generated files in the same folder: [build/src](build/src)).

### 3.4. Peripheral

The periphery initialization is done in main() function of [Libs/stm32-cube-project/Core/Src/main.c](https://github.com/RaccoonLabHardware/mini-v2-software/blob/main/Core/Src/main.c) file. This file as well as all peripheral related drivers are pre-generated based on the configuratation .ioc file called [project.ioc](https://github.com/RaccoonLabHardware/mini-v2-software/blob/main/can_pwm_v2.ioc). If you want to use a different peripheral configuration, you should update the .ioc with STM32CubeIDE or STM32CubeMX and regenerate the project.

The periphery drivers are divided into 2 groups.

The drivers interfaces are represented by classes defined in header files in [Src/periphery] folder as shown below:

<img src="https://raw.githubusercontent.com/wiki/RaccoonlabDev/mini_v2_node/assets/periphery.png" alt="drawing">

An actual implementation toy can find in source files in [Src/platform] folder. Depending in which mode you are building the application (SITL ubuntu or stm32f103) and the board type (mini v2 or a custom one) it uses different implementations:
- stm32 implementations depend on STM32CubeMX generated HAL drivers (though it is not required and you can write your optimized implementation),
- Ubuntu drivers implementations usually are just a mock drivers.

### 3.5. Modules

A module is a specific independent unit of an application functionality. It can be implemented in a different way, for example as RTOS task. But let's keep it baremetal for a while because typically (but not always) a Cyphal/DroneCAN application is simple and resource limited.

An example of modules for a minimal CAN-PWM application is shown below:

<img src="https://raw.githubusercontent.com/wiki/RaccoonlabDev/mini_v2_node/assets/cyphal_modules.png" alt="drawing">

Typically, we recommend having at least CircuitStatus module that is responsible for internal status measurements including:
- 5V (after DC-DC),
- Vin (before DC-DC),
- STM32 internal temperature,
- Hardware version (from ADC_VERSION).

Some boards can have a current sensor and external temperature sensor or other ADC sensors.

Each module is expected to have a yaml file with his parameters.

### 3.6. Application entry point

After the peripheral initialization the application goes to the `application_entry_point()` in [Src/common/application.cpp](Src/common/application.cpp). It is assumed that a user will provide a custom application here.

## 4. Running SITL mode

**4.1. Clone the repo with submodules**

```bash
git clone https://github.com/RaccoonlabDev/mini_v2_node --recursive
cd mini_v2_node
git submodule update --init --recursive
```

**4.2. Build the project and run the firmware**

```bash
make sitl_dronecan run
```

**4.3. Run gui_tool**

```bash
dronecan_gui_tool
```

## 5. Building and uploading the provided application

### 5.1. Clone the repo with submodules

```bash
git clone https://github.com/RaccoonlabDev/mini_v2_node --recursive
cd mini_v2_node
git submodule update --init --recursive
```

### 5.2. Connect Sniffer and Programmer to Mini v2 node

An example of connection scheme suitable for bench test for Mini v2 node and RL Programmer-Sniffer is shown below:

<img src="https://raw.githubusercontent.com/wiki/RaccoonlabDev/mini_v2_node/assets/connection.png" alt="drawing">

> You can also use other sniffer and programmers. For details refer to: [Programmer usage](https://docs.raccoonlab.co/guide/programmer_sniffer/programmer.html) and [Sniffer usage](https://docs.raccoonlab.co/guide/programmer_sniffer/sniffer.html) pages.

### 5.3. Build the project and upload the firmware

```bash
make dronecan upload
```

### 5.4. Run gui_tool

Run gui_tool.

Please, refer to the [Mini node docs](https://docs.raccoonlab.co/guide/can_pwm/can_pwm_dronecan.html).
It has a detailed steps about how to perform bench testing of the node.

## 6. Customizing the application

> The following steps assume that you are already successful with the default application provided in this repository and want to customize it.

### 6.1. Make a template or fork

Make a template or fork of the repository. Then clone it. Don't forget about the submodules.

```bash
git clone https://github.com/RaccoonlabDev/mini_v2_node --recursive
cd mini_v2_node
git submodule update --init --recursive
```

> Here, instead of RaccoonlabDev you should type the name of your org or profile.

### 6.2. (optional) Modify the repo

1. Optionally remove everything related to Cyphal:

```bash
rm .github/workflows/cyphal_sitl.yml
rm -rf Src/modules/cyphal Src/modules/feedback/cyphal Src/modules/rcout/cyphal_frontend
rm -rf Src/applications/cyphal
```

2. Update README.md with description of your application

### 6.3. Modify periphery config

Mini v2 node has 6 user's pins. By default 4 of them are configured as PWM and 2 of them as UART RX. The pinout configuration is shown on the image below:

| Pinout | Default config |
| ------ | -------------- |
| <img src="https://docs.raccoonlab.co/assets/img/pinout.c14a3021.png" alt="drawing" width="315"> | <img src="https://raw.githubusercontent.com/RaccoonLabHardware/mini_v2_stm32cubemx_project/main/Assets/stm32cubemx.png" alt="drawing" width="400"> |

If you want to use a custom periphery configuration, update the [Libs/stm32-cube-project/project.ioc](Libs/stm32-cube-project/project.ioc) file with STM32CubeIDE or STM32CubeMX and regenerate the project.

Since [Libs/stm32-cube-project](Libs/stm32-cube-project) is submodule, you can either use your own submodule (make a fork or template [RaccoonLabHardware/mini-v2-software](https://github.com/RaccoonLabHardware/mini-v2-software)) or just remove the submodule and include the source code in the root repository.

### 6.4. Modify the application

The main application is started in [Src/common/application.cpp](Src/common/application.cpp).
By default it just blinks the RGB LED, subscribes to the setpoint topic to control PWM1 and publishes a feedback with the latest applied setpoint.

> Note, that the application is as simple as possible: it controls only a single PWM and doesn't have safety features like TTL, but you are free to extend it as you want.

### 6.5. Add custom DroneCAN publisher

1. Include a header file, for example:

```c++
#include "dronecan.h"
#include "uavcan/equipment/power/BatteryInfo.h"
```

2. Create a message and trasnfer id, for example:

```c++
BatteryInfo_t battery_info{};
static uint8_t transfer_id = 0;
```

3. Publish a message and increase the `transfer_id`, for example:

```c++
dronecan_equipment_battery_info_publish(&battery_info, &transfer_id);
transfer_id++;
```

### 6.6. Add custom DroneCAN subscriber

```c++
#include "dronecan.h"
#include "uavcan/equipment/indication/LightsCommand.h"

void callback(CanardRxTransfer* transfer) {
    LightsCommand_t lights_command;
    int8_t res = dronecan_equipment_indication_lights_command_deserialize(transfer, &lights_command);
    if (res < 0) {
        // handle error
    }

    // do something with the received command
}

// Somewhere on the initialization step:
if (uavcanSubscribe(UAVCAN_EQUIPMENT_INDICATION_LIGHTS_COMMAND, callback) < 0) {
    // handle error
}
```

### 6.7. Add custom parameters

> This section is in progress. Please, check examples or libparams for the details.

When you add your custom module, don't forget to add source file and path to the file with registers to [Src/applications/dronecan/CMakeLists.txt](Src/applications/dronecan/CMakeLists.txt).

You can also easily create custom Integer and String registers. An example is shown in [Src/modules/system/params_common.yaml](Src/modules/system/params_common.yaml).

### 6.8. Build and upload

```bash
make dronecan upload
```
