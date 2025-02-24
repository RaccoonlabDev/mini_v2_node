/// This software is distributed under the terms of the MIT License.
/// Copyright (c) 2022 Dmitry Ponomarev.
/// Author: Dmitry Ponomarev <ponomarevda96@gmail.com>

#include "application.hpp"
#include <cstdlib>
#include "kocherga.hpp"
#include "my_can_driver.hpp"
#include "kocherga_can.hpp"
#include "main.h"
#include "cyphalNode/cyphal.hpp"
#include "libparams_error_codes.h"
#include "params.hpp"
#include "flash_driver.h"
#include "rom.h"

// Please, specify config here:
#define BOOTLOADER_SIZE_PAGES       32

// Automatically calculated:
#define RESERVED_PAGES              3
#define BOOTLOADER_SIZE_BYTES       (flashGetPageSize() * BOOTLOADER_SIZE_PAGES)
#define APP_FIRST_PAGE_IDX          BOOTLOADER_SIZE_PAGES
#define APP_START_ADDRESS           (FLASH_START_ADDR + BOOTLOADER_SIZE_BYTES)
#define APP_MAX_SIZE_PAGES      (flashGetNumberOfPages() - BOOTLOADER_SIZE_PAGES - RESERVED_PAGES)
#define APP_MAX_SIZE_BYTES          (APP_MAX_SIZE_PAGES * flashGetPageSize())

#define PARAMS_SECTION_REL_ADDR     (APP_MAX_SIZE_PAGES + 2) * flashGetPageSize()
#define RESERVED_SECTION_REL_ADDR   (APP_MAX_SIZE_PAGES + 1) * flashGetPageSize()

class MyROMBackend final : public kocherga::IROMBackend {
public:
    MyROMBackend()
    {
        rom = romInit(APP_FIRST_PAGE_IDX, APP_MAX_SIZE_PAGES + RESERVED_PAGES);
    }

private:
    auto write(const std::size_t offset, const std::byte* const data, const std::size_t size)
        -> std::optional<std::size_t> override
    {
        if (romWrite(&rom, offset, (const uint8_t*)data, size)) {
            return size;
        }
        return {};
    }

    auto read(const std::size_t offset, std::byte* const out_data, const std::size_t size) const
        -> std::size_t override
    {
        return romRead(&rom, offset, (uint8_t*)out_data, size);
    }

    void beginWrite() override
    {
        romBeginWrite(&rom);
    }

    void endWrite() override
    {
        romEndWrite(&rom);
    }

    static inline RomDriverInstance rom;
};


#define RED_PORT    INTERNAL_LED_RED_GPIO_Port
#define RED_PIN     INTERNAL_LED_RED_Pin
#define GREEN_PORT  INTERNAL_LED_GREEN_GPIO_Port
#define GREEN_PIN   INTERNAL_LED_GREEN_Pin
#define BLUE_PORT   INTERNAL_LED_BLUE_GPIO_Port
#define BLUE_PIN    INTERNAL_LED_BLUE_Pin

enum class BootloaderCommand {
    UNKNOWN_BOOT = 0,
    FORCE_RUN_CYPHAL_APPLICATION = 1,
    FORCE_RUN_DRONECAN_APPLICATION = 2,
};

void boot_app() {
    typedef void (*pFunction)(void);
    __disable_irq();
    uint32_t app_jump_address = *( uint32_t*) (APP_START_ADDRESS + 4);
    pFunction Jump_To_Application = (pFunction)app_jump_address;
    SCB->VTOR = APP_START_ADDRESS;
    __set_MSP(*(__IO uint32_t*) APP_START_ADDRESS);
    Jump_To_Application();
}

void reboot() {
    HAL_NVIC_SystemReset();
}

void ledsTurnOffAll() {
    HAL_GPIO_WritePin(RED_PORT,   RED_PIN,   GPIO_PIN_SET);
    HAL_GPIO_WritePin(GREEN_PORT, GREEN_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(BLUE_PORT,  BLUE_PIN,  GPIO_PIN_SET);
}

GPIO_PinState timeToPinState(uint32_t period) {
    return (HAL_GetTick() % period) > (period / 2) ? GPIO_PIN_SET : GPIO_PIN_RESET;
}

void ledsToggleBlue(uint32_t period = 0) {
    if (period == 0) {
        HAL_GPIO_TogglePin(BLUE_PORT, BLUE_PIN);
    } else {
        HAL_GPIO_WritePin(BLUE_PORT, BLUE_PIN, timeToPinState(period));
    }
}

void ledsToggleRed(uint32_t period = 0) {
    if (period == 0) {
        HAL_GPIO_TogglePin(RED_PORT, RED_PIN);
    } else {
        HAL_GPIO_WritePin(RED_PORT, RED_PIN, timeToPinState(period));
    }
}

void ledsToggleGreen(uint32_t period = 0) {
    if (period == 0) {
        HAL_GPIO_TogglePin(GREEN_PORT, GREEN_PIN);
    } else {
        HAL_GPIO_WritePin(GREEN_PORT, GREEN_PIN, timeToPinState(period));
    }
}

void ledsTogglePurple(uint32_t period = 0) {
    if (period == 0) {
        HAL_GPIO_TogglePin(BLUE_PORT, BLUE_PIN);
        HAL_GPIO_TogglePin(RED_PORT, RED_PIN);
    } else {
        auto pin_state = timeToPinState(period);
        HAL_GPIO_WritePin(BLUE_PORT, BLUE_PIN, pin_state);
        HAL_GPIO_WritePin(RED_PORT, RED_PIN, pin_state);
    }
}

void catch_error(uint32_t period_ms) {
    ledsTurnOffAll();

    uint32_t next_toggle_led_ts_ms = 500;
    uint32_t crnt_ts_ms = 0;
    while (true) {
        crnt_ts_ms = HAL_GetTick();
        if (crnt_ts_ms > next_toggle_led_ts_ms) {
            next_toggle_led_ts_ms += period_ms;
            ledsToggleRed();
        }
    }
}

void assert_true(bool is_true) {
    if (!is_true) {
        catch_error(100);
    }
}

uint8_t determine_node_id() {
    auto node_id = 42;  // paramsGetIntegerValue(PARAM_NODE_ID);

    if (node_id < 1 || node_id >= 127) {
        node_id = 42;
    }

    return node_id;
}

uint8_t determine_protocol_version(MyCANDriver& can_driver) {
    ledsTurnOffAll();
    uint8_t protocol_version;

    auto cmd = static_cast<BootloaderCommand>(paramsGetIntegerValue(PARAM_BOOTLOADER_INTERNAL));
    kocherga::can::ICANDriver::PayloadBuffer payload_buffer;
    if (cmd == BootloaderCommand::UNKNOWN_BOOT) {
        while (true) {
            auto frame = can_driver.pop(payload_buffer);
            ledsToggleBlue(500);
            if (frame == std::nullopt || frame->second == 0) {
                continue;
            }
            ledsToggleGreen();

            uint8_t last_byte_idx = frame->second - 1;
            uint8_t tail_byte_without_transfer_id = payload_buffer[last_byte_idx] & 0b11100000;
            if (tail_byte_without_transfer_id == 0b11000000) {
                protocol_version = 0;
                break;
            } else if (tail_byte_without_transfer_id == 0b11100000) {
                protocol_version = 1;
                break;
            }
        }
    } else if (cmd == BootloaderCommand::FORCE_RUN_DRONECAN_APPLICATION) {
        protocol_version = 1;
    } else if (cmd == BootloaderCommand::FORCE_RUN_CYPHAL_APPLICATION) {
        protocol_version = 1;
    } else {
        protocol_version = 0;
    }

    return protocol_version;
}

void bootloader_entry_point() {
    ledsTurnOffAll();

    paramsInit(2, 1, -1, 1);
    paramsLoad();

    MyROMBackend rom_backend;

    kocherga::SystemInfo system_info = {
        .node_name = "Bootloader"
    };

    MyCANDriver can_driver;
    can_driver.init();
    kocherga::can::CANNode can_node(
        can_driver,
        system_info.unique_id,
        kocherga::can::ICANDriver::Bitrate{1000000, 1000000},
        determine_protocol_version(can_driver),
        determine_node_id()
    );

    kocherga::Bootloader bootloader(
        rom_backend,
        system_info,
        APP_MAX_SIZE_BYTES,     ///< max_app_size
        false,                  ///< linger
        std::chrono::seconds(10)
    );
    assert_true(bootloader.addNode(&can_node));

    ledsTurnOffAll();
    while (true) {
        const auto uptime = GET_TIME_SINCE_BOOT();
        const auto fin = bootloader.poll(
            std::chrono::duration_cast<std::chrono::microseconds>(uptime)
        );

        auto state = bootloader.getState();
        if (state == kocherga::State::NoAppToBoot) {
            ledsTogglePurple(100);
        } else if (state == kocherga::State::BootDelay) {
            ledsToggleGreen(100);
        } else if (state == kocherga::State::BootCanceled) {
            ledsToggleBlue(100);
        } else if (state == kocherga::State::AppUpdateInProgress) {
            ledsToggleRed(100);
        }

        uint32_t crnt_time_ms = HAL_GetTick();
        if (crnt_time_ms > 10000 && *fin == kocherga::Final::BootApp) {
            boot_app();
        } else if (*fin == kocherga::Final::Restart) {
            reboot();
        }
    }
}
