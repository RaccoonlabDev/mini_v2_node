/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Anastasiia Stepanova <asiiapine@gmail.com>
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "PWMModule.hpp"
#include "uavcan/equipment/hardpoint/Command.h"
#include "uavcan/equipment/hardpoint/Status.h"
#include "uavcan/equipment/indication/LightsCommand.h"
#include "uavcan/equipment/esc/RawCommand.h"
#include "uavcan/equipment/esc/Status.h"
#include "uavcan/equipment/actuator/ArrayCommand.h"
#include "uavcan/equipment/actuator/Status.h"

#define CHANNEL(channel) IntParamsIndexes::PARAM_PWM_##channel##_CH
#define MIN(channel) IntParamsIndexes::PARAM_PWM_##channel##_MIN
#define MAX(channel) IntParamsIndexes::PARAM_PWM_##channel##_MAX
#define DEF(channel) IntParamsIndexes::PARAM_PWM_##channel##_DEF
#define FB(channel) IntParamsIndexes::PARAM_PWM_##channel##_FB

static constexpr uint16_t CMD_RELEASE_OR_MIN = 0;
static constexpr uint16_t CMD_HOLD_OR_MAX = 1;

Logger PWMModule::logger = Logger("PWMModule");

uint16_t PWMModule::ttl_cmd = 0;
uint16_t PWMModule::pwm_freq = 1000;
CommandType PWMModule::pwm_cmd_type = CommandType::RAW_COMMAND;

ModuleStatus PWMModule::module_status = ModuleStatus::MODULE_OK;

PWMModule PWMModule::instance = PWMModule();

PWMModule::PWMModule() {
    update_params();
    init();
}

std::array<PwmChannelInfo, static_cast<uint8_t>(PwmPin::PWM_AMOUNT)> PWMModule::params = {{
    {.pin = PwmPin::PWM_1},  // PWM1
    {.pin = PwmPin::PWM_2},  // PWM2
    {.pin = PwmPin::PWM_3},  // PWM3
    {.pin = PwmPin::PWM_4},  // PWM4
}};

PwmChannelsParamsNames PWMModule::params_names[static_cast<uint8_t>(PwmPin::PWM_AMOUNT)] = {
    {.min = MIN(1), .max = MAX(1), .def = DEF(1), .ch = CHANNEL(1), .fb = FB(1)},  // PWM1
    {.min = MIN(2), .max = MAX(2), .def = DEF(2), .ch = CHANNEL(2), .fb = FB(2)},  // PWM2
    {.min = MIN(3), .max = MAX(3), .def = DEF(3), .ch = CHANNEL(3), .fb = FB(3)},  // PWM3
    {.min = MIN(4), .max = MAX(4), .def = DEF(4), .ch = CHANNEL(4), .fb = FB(4)},  // PWM4
};

PWMModule& PWMModule::get_instance() {
    static bool instance_initialized = false;
    if (!instance_initialized) {
        instance_initialized = true;
        instance.init();
    }
    return instance;
}

void PWMModule::init() {
    logger.init("PWMModule");
    for (int i = 0; i < static_cast<uint8_t>(PwmPin::PWM_AMOUNT); i++) {
        PwmPeriphery::init(params[i].pin);
    }

    uavcanSubscribe(UAVCAN_EQUIPMENT_ESC_RAWCOMMAND,            raw_command_callback);
    uavcanSubscribe(UAVCAN_EQUIPMENT_ACTUATOR_ARRAY_COMMAND,    array_command_callback);
    uavcanSubscribe(UAVCAN_EQUIPMENT_HARDPOINT_COMMAND,         hardpoint_callback);
}

void PWMModule::spin_once() {
    uint32_t crnt_time_ms = HAL_GetTick();

    static uint32_t next_update_ms = 0;
    if (crnt_time_ms > next_update_ms) {
        next_update_ms = crnt_time_ms + 1000;
        instance.update_params();
        instance.apply_params();
    }

    for (int i = 0; i < static_cast<uint8_t>(PwmPin::PWM_AMOUNT); i++) {
        auto pwm = params[i];
        if (crnt_time_ms > pwm.cmd_end_time_ms) {
            pwm.command_val = pwm.def;
        }
        PwmPeriphery::set_duration(pwm.pin, pwm.command_val);
    }

    status_pub_timeout_ms = 1;
    static uint32_t next_pub_ms = 5000;

    if (module_status == ModuleStatus::MODULE_OK && crnt_time_ms > next_pub_ms) {
        publish_state();
        next_pub_ms = crnt_time_ms + status_pub_timeout_ms;
    }
}

void PWMModule::update_params() {
    module_status = ModuleStatus::MODULE_OK;

    pwm_freq = paramsGetIntegerValue(IntParamsIndexes::PARAM_PWM_FREQUENCY);
    pwm_cmd_type = (CommandType)paramsGetIntegerValue(IntParamsIndexes::PARAM_PWM_CMD_TYPE);

    ttl_cmd = paramsGetIntegerValue(IntParamsIndexes::PARAM_PWM_CMD_TTL_MS);
    status_pub_timeout_ms = 100;
    uint8_t max_channel = 0;

    bool params_error = false;
    switch (pwm_cmd_type) {
        case CommandType::RAW_COMMAND:
            max_channel = NUMBER_OF_RAW_CMD_CHANNELS - 1;
            break;
        case CommandType::ARRAY_COMMAND:
            max_channel = 255;
            break;
        case CommandType::HARDPOINT_COMMAND:
            max_channel = 255;
            break;
        default:
            params_error = true;
            break;
    }

    static uint32_t last_warn_pub_time_ms = 0;
    for (int i = 0; i < static_cast<uint8_t>(PwmPin::PWM_AMOUNT); i++) {
        params[i].fb = paramsGetIntegerValue(params_names[i].fb);
        auto channel = paramsGetIntegerValue(params_names[i].ch);
        if (channel < max_channel) {
            params[i].channel = channel;
        } else {
            params[i].channel = max_channel;
            params_error = true;
        }

        params[i].def = paramsGetIntegerValue(params_names[i].def);
        params[i].min = paramsGetIntegerValue(params_names[i].min);
        params[i].max = paramsGetIntegerValue(params_names[i].max);
    }

    if (params_error) {
        module_status = ModuleStatus::MODULE_WARN;
        if (last_warn_pub_time_ms < HAL_GetTick()) {
            last_warn_pub_time_ms = HAL_GetTick() + 10000;
            logger.log_debug("check parameters");
        }
    }
}

void PWMModule::apply_params() {
    for (int i = 0; i < static_cast<uint8_t>(PwmPin::PWM_AMOUNT); i++) {
        if (PwmPeriphery::get_frequency(params[i].pin) != pwm_freq) {
            PwmPeriphery::set_frequency(params[i].pin, pwm_freq);
        }
    }

    switch (pwm_cmd_type) {
        case CommandType::RAW_COMMAND:
            publish_state = publish_esc_status;
            break;
        case CommandType::ARRAY_COMMAND:
            publish_state = publish_actuator_status;
            break;
        case CommandType::HARDPOINT_COMMAND:
            publish_state = publish_hardpoint_status;
            break;
        default:
            break;
    }
}

void PWMModule::publish_esc_status() {
    static uint8_t transfer_id = 0;
    EscStatus_t msg{};
    auto crnt_time_ms = HAL_GetTick();
    static uint32_t next_status_pub_ms[static_cast<uint8_t>(PwmPin::PWM_AMOUNT)];
    for (int i = 0; i < static_cast<uint8_t>(PwmPin::PWM_AMOUNT); i++) {
        auto pwm = params[i];
        if (pwm.channel < 0 || pwm.fb == 0 ||
            next_status_pub_ms[i] > crnt_time_ms) {
            continue;
        }
        msg = {};
        msg.esc_index = pwm.channel;
        auto pwm_val = PwmPeriphery::get_duration(pwm.pin);
        auto scaled_value = mapPwmToPct(pwm_val, pwm.min, pwm.max);
        msg.power_rating_pct = (uint8_t)(scaled_value);
        if (dronecan_equipment_esc_status_publish(&msg, &transfer_id) == 0) {
            transfer_id++;
            next_status_pub_ms[i] = crnt_time_ms + ((pwm.fb > 1) ? 100 : 1000);
        }
    }
}

void PWMModule::publish_actuator_status() {
    static uint8_t transfer_id = 0;
    ActuatorStatus_t msg {};
    auto crnt_time_ms = HAL_GetTick();
    static uint32_t next_status_pub_ms[static_cast<uint8_t>(PwmPin::PWM_AMOUNT)];
    for (int i =0; i < static_cast<uint8_t>(PwmPin::PWM_AMOUNT); i++) {
        auto pwm = params[i];
        if (pwm.channel < 0 || pwm.fb == 0 ||
            next_status_pub_ms[i] > crnt_time_ms) {
            continue;
        }
        msg.actuator_id = pwm.channel;
        auto pwm_val = PwmPeriphery::get_duration(pwm.pin);
        auto scaled_value = mapPwmToPct(pwm_val, pwm.min, pwm.max);
        msg.power_rating_pct = (uint8_t) scaled_value;
        if (dronecan_equipment_actuator_status_publish(&msg, &transfer_id) == 0) {
            transfer_id++;
        }
        next_status_pub_ms[i] = crnt_time_ms + ((pwm.fb > 1) ? 100 : 1000);
    }
}

void PWMModule::publish_hardpoint_status() {
    static uint32_t next_status_pub_ms{0};
    static uint8_t transfer_id{0};

    auto crnt_time_ms = HAL_GetTick();
    if (next_status_pub_ms > crnt_time_ms) {
        return;
    }
    next_status_pub_ms = crnt_time_ms + 1000;

    for (auto& pwm : params) {
        if (pwm.channel < 0) {
            continue;
        }

        HardpointStatus msg{};
        msg.hardpoint_id = pwm.channel;
        auto pwm_duration_us = PwmPeriphery::get_duration(pwm.pin);
        msg.status = (pwm_duration_us == pwm.min) ? CMD_RELEASE_OR_MIN : CMD_HOLD_OR_MAX;

        if (dronecan_equipment_hardpoint_status_publish(&msg, &transfer_id) == 0) {
            transfer_id++;
        }
    }
}

void PWMModule::raw_command_callback(CanardRxTransfer* transfer) {
    if (module_status != ModuleStatus::MODULE_OK || pwm_cmd_type != CommandType::RAW_COMMAND) {
        return;
    }

    for (auto& pwm : params) {
        int16_t cmd;
        if (!dronecan_equipment_esc_raw_command_channel_deserialize(transfer, pwm.channel, &cmd)) {
            continue;
        }

        pwm.cmd_end_time_ms = HAL_GetTick() + ttl_cmd;
        pwm.command_val = mapRawCommandToPwm(cmd, pwm.min, pwm.max, pwm.def);
    }
}

void PWMModule::array_command_callback(CanardRxTransfer* transfer) {
    if (module_status != ModuleStatus::MODULE_OK || pwm_cmd_type != CommandType::ARRAY_COMMAND) {
        return;
    }

    ArrayCommand_t command;
    int8_t ch_num = dronecan_equipment_actuator_arraycommand_deserialize(transfer, &command);
    if (ch_num <= 0) {
        return;
    }
    for (int i = 0; i < static_cast<uint8_t>(PwmPin::PWM_AMOUNT); i++) {
        auto pwm = &params[i];
        if (pwm->channel < 0) {
            continue;
        }
        for (uint8_t j = 0; j < ch_num; j++) {
            if (command.commads[j].actuator_id != pwm->channel) {
                continue;
            }
            pwm->cmd_end_time_ms = HAL_GetTick() + ttl_cmd;
            pwm->command_val = mapActuatorCommandToPwm(command.commads[j].command_value,
                                                         pwm->min, pwm->max, pwm->def);
        }
    }
}

void PWMModule::hardpoint_callback(CanardRxTransfer* transfer) {
    if (module_status != ModuleStatus::MODULE_OK ||
            pwm_cmd_type != CommandType::HARDPOINT_COMMAND) {
        return;
    }

    HardpointCommand cmd;
    if (!dronecan_equipment_hardpoint_command_deserialize(transfer, &cmd)) {
        return;
    }

    for (auto& pwm : params) {
        if (cmd.hardpoint_id != pwm.channel) {
            continue;
        }

        pwm.cmd_end_time_ms = HAL_GetTick() + ttl_cmd;
        pwm.command_val = (cmd.command == CMD_HOLD_OR_MAX) ? pwm.max : pwm.min;
    }
}
