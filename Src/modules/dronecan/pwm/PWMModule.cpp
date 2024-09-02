/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Anastasiia Stepanova <asiiapine@gmail.com>
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "PWMModule.hpp"
#include <limits>
#include "uavcan/equipment/hardpoint/Command.h"
#include "uavcan/equipment/hardpoint/Status.h"
#include "uavcan/equipment/indication/LightsCommand.h"
#include "uavcan/equipment/esc/RawCommand.h"
#include "uavcan/equipment/esc/Status.h"
#include "uavcan/equipment/actuator/ArrayCommand.h"
#include "uavcan/equipment/actuator/Status.h"
#include "periphery/adc/circuit_periphery.hpp"
#include "common/algorithms.hpp"

#define CH(channel) IntParamsIndexes::PARAM_PWM_##channel##_CH
#define MIN(channel) IntParamsIndexes::PARAM_PWM_##channel##_MIN
#define MAX(channel) IntParamsIndexes::PARAM_PWM_##channel##_MAX
#define DEF(channel) IntParamsIndexes::PARAM_PWM_##channel##_DEF
#define FB(channel) IntParamsIndexes::PARAM_PWM_##channel##_FB

static constexpr uint16_t CMD_RELEASE_OR_MIN = 0;
static constexpr uint16_t CMD_HOLD_OR_MAX = 1;

Logger PWMModule::logger = Logger("PWMModule");

uint16_t PWMModule::ttl_cmd = 500;
uint16_t PWMModule::pwm_freq = 50;
CommandType PWMModule::pwm_cmd_type = CommandType::RAW_COMMAND;

REGISTER_MODULE(PWMModule)

std::array<PwmChannelInfo, static_cast<uint8_t>(HAL::PwmPin::PWM_AMOUNT)> PWMModule::params = {{
    {{.min = MIN(1), .max = MAX(1), .def = DEF(1), .ch = CH(1), .fb = FB(1)}, HAL::PwmPin::PWM_1},
    {{.min = MIN(2), .max = MAX(2), .def = DEF(2), .ch = CH(2), .fb = FB(2)}, HAL::PwmPin::PWM_2},
    {{.min = MIN(3), .max = MAX(3), .def = DEF(3), .ch = CH(3), .fb = FB(3)}, HAL::PwmPin::PWM_3},
    {{.min = MIN(4), .max = MAX(4), .def = DEF(4), .ch = CH(4), .fb = FB(4)}, HAL::PwmPin::PWM_4},
}};

void PWMModule::init() {
    mode = Module::Mode::INITIALIZATION;

    update_params();

    logger.init("PWMModule");
    for (auto param : params) {
        HAL::Pwm::init(param.pin);
    }

    raw_command_sub.init(raw_command_cb);
    uavcanSubscribe(UAVCAN_EQUIPMENT_ACTUATOR_ARRAY_COMMAND,    array_command_callback);
    uavcanSubscribe(UAVCAN_EQUIPMENT_HARDPOINT_COMMAND,         hardpoint_callback);
}

void PWMModule::spin_once() {
    uint32_t crnt_time_ms = HAL_GetTick();

    mode = Module::Mode::STANDBY;
    for (auto& pwm : params) {
        if (crnt_time_ms > pwm.cmd_end_time_ms) {
            pwm.command_val = pwm.def;
            pwm.is_engaged = false;
        }
        HAL::Pwm::set_duration(pwm.pin, pwm.command_val);
        if (pwm.is_engaged) {
            mode = Mode::ENGAGED;
        }
    }
    status_pub_timeout_ms = 1;
    static uint32_t next_pub_ms = 5000;

    if (health == Status::OK && crnt_time_ms > next_pub_ms) {
        publish_state();
        next_pub_ms = crnt_time_ms + status_pub_timeout_ms;
    }
}

void PWMModule::update_params() {
    health = Status::OK;
    mode = Mode::MAINTENANCE;
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
    if (health != Status::OK&& HAL_GetTick() %1000 == 0) {
        logger.log_info("102");
    }
    static uint32_t last_warn_pub_time_ms = 0;
    for (int i = 0; i < static_cast<uint8_t>(HAL::PwmPin::PWM_AMOUNT); i++) {
        params[i].fb = paramsGetIntegerValue(params[i].names.fb);
        auto channel = paramsGetIntegerValue(params[i].names.ch);
        if (channel < max_channel) {
            params[i].channel = channel;
        } else {
            params[i].channel = max_channel;
            params_error = true;
        }

        params[i].def = paramsGetIntegerValue(params[i].names.def);
        params[i].min = paramsGetIntegerValue(params[i].names.min);
        params[i].max = paramsGetIntegerValue(params[i].names.max);
    }

    if (params_error) {
        health = Status::MINOR_FAILURE;
        if (last_warn_pub_time_ms < HAL_GetTick()) {
            last_warn_pub_time_ms = HAL_GetTick() + 10000;
            logger.log_debug("check parameters");
        }
    }
    if (health != Status::OK && HAL_GetTick() %1000 == 0) {
        logger.log_info("128");
    }
    apply_params();
    mode = Mode::STANDBY;
}

void PWMModule::apply_params() {
    for (int i = 0; i < static_cast<uint8_t>(HAL::PwmPin::PWM_AMOUNT); i++) {
        if (HAL::Pwm::get_frequency(params[i].pin) != pwm_freq) {
            HAL::Pwm::set_frequency(params[i].pin, pwm_freq);
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
    for (auto& pwm : params) {
        auto crnt_time_ms = HAL_GetTick();
        if (pwm.channel < 0 || pwm.fb == 0 || pwm.next_status_pub_ms > crnt_time_ms) {
            continue;
        }

        auto& msg = esc_status_pub.msg;
        msg.esc_index = pwm.channel;
        auto pwm_val = HAL::Pwm::get_duration(pwm.pin);
        auto scaled_value = mapPwmToPct(pwm_val, pwm.min, pwm.max);
        msg.power_rating_pct = (uint8_t)(scaled_value);
        esc_status_pub.publish();

        pwm.next_status_pub_ms = crnt_time_ms + ((pwm.fb > 1) ? 100 : 1000);
    }
}

void PWMModule::publish_actuator_status() {
    for (auto& pwm : params) {
        auto crnt_time_ms = HAL_GetTick();
        if (pwm.channel < 0 || pwm.fb == 0 || pwm.next_status_pub_ms > crnt_time_ms) {
            continue;
        }

        auto percent = (uint8_t)mapPwmToPct(HAL::Pwm::get_duration(pwm.pin), pwm.min, pwm.max);

        auto& msg = actuator_status_pub.msg;
        msg.actuator_id = pwm.channel;
        msg.power_rating_pct = percent;

        // The following fields are not used in PX4 anyway
        // Let's fill them with something useful for logging for a while
        msg.force = CircuitPeriphery::current();
        msg.speed = CircuitPeriphery::temperature();
        msg.position = CircuitPeriphery::voltage_5v();

        actuator_status_pub.publish();

        // We don't want to publish this message often, probably 1 Hz is fine
        // We want to record the maximum current, the highest temperature and the lowest voltage
        // As a temporary solution, let's publish with 2 Hz
        pwm.next_status_pub_ms = crnt_time_ms + 500;
    }
}

void PWMModule::publish_hardpoint_status() {
    static uint32_t next_status_pub_ms{0};

    auto crnt_time_ms = HAL_GetTick();
    if (next_status_pub_ms > crnt_time_ms) {
        return;
    }
    next_status_pub_ms = crnt_time_ms + 1000;

    for (auto& pwm : params) {
        if (pwm.channel < 0) {
            continue;
        }

        auto& msg = hardpoint_status_pub.msg;
        msg.hardpoint_id = pwm.channel;
        auto pwm_duration_us = HAL::Pwm::get_duration(pwm.pin);
        msg.status = (pwm_duration_us == pwm.min) ? CMD_RELEASE_OR_MIN : CMD_HOLD_OR_MAX;

        hardpoint_status_pub.publish();
    }
}

void PWMModule::raw_command_cb(const RawCommand_t& msg) {
    if (pwm_cmd_type != CommandType::RAW_COMMAND) {
        return;
    }

    for (auto& pwm : params) {
        if (pwm.channel >= msg.size || pwm.channel < 0) {
            continue;
        }
        if (msg.raw_cmd[pwm.channel] > -1) {
            pwm.is_engaged = true;
        } else {
            pwm.is_engaged = false;
        }
        pwm.cmd_end_time_ms = HAL_GetTick() + ttl_cmd;
        pwm.command_val = mapInt16CommandToPwm(msg.raw_cmd[pwm.channel], pwm.min, pwm.max, pwm.def);
    }
}

void PWMModule::array_command_callback(CanardRxTransfer* transfer) {
    if (pwm_cmd_type != CommandType::ARRAY_COMMAND) {
        return;
    }

    ArrayCommand_t command;
    int8_t ch_num = dronecan_equipment_actuator_arraycommand_deserialize(transfer, &command);
    if (ch_num <= 0) {
        return;
    }
    for (int i = 0; i < static_cast<uint8_t>(HAL::PwmPin::PWM_AMOUNT); i++) {
        auto pwm = &params[i];
        if (pwm->channel < 0) {
            continue;
        }
        for (uint8_t j = 0; j < ch_num; j++) {
            if (command.commads[j].actuator_id != pwm->channel) {
                continue;
            }
            pwm->is_engaged = true;
            pwm->cmd_end_time_ms = HAL_GetTick() + ttl_cmd;
            pwm->command_val = mapFloatCommandToPwm(command.commads[j].command_value,
                                                    pwm->min, pwm->max, pwm->def);
        }
    }
}

void PWMModule::hardpoint_callback(CanardRxTransfer* transfer) {
    if (pwm_cmd_type != CommandType::HARDPOINT_COMMAND) {
        return;
    }

    HardpointCommand cmd;
    if (dronecan_equipment_hardpoint_command_deserialize(transfer, &cmd) < 0) {
        return;
    }

    for (auto& pwm : params) {
        if (cmd.hardpoint_id != pwm.channel) {
            continue;
        }
        pwm.is_engaged = true;
        // TTL has no effect on Hardpoint
        pwm.cmd_end_time_ms = std::numeric_limits<decltype(pwm.cmd_end_time_ms)>::max();
        pwm.command_val = (cmd.command == CMD_HOLD_OR_MAX) ? pwm.max : pwm.min;
    }
}

void PWMModule::arming_status_callback(const ArmingStatus& msg) {
    if (msg == ArmingStatus::STATUS_FULLY_ARMED) {
        // params[0].is_engaged = true;
    }
}
