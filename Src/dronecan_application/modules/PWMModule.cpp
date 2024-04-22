#include "PWMModule.hpp"

#define CHANNEL(channel) IntParamsIndexes::PARAM_PWM_##channel##_CH
#define MIN(channel) IntParamsIndexes::PARAM_PWM_##channel##_MIN
#define MAX(channel) IntParamsIndexes::PARAM_PWM_##channel##_MAX
#define DEF(channel) IntParamsIndexes::PARAM_PWM_##channel##_DEF
#define FB(channel) IntParamsIndexes::PARAM_PWM_##channel##_FB

Logger PWMModule::logger = Logger("PWMModule");

uint16_t PWMModule::ttl_cmd = 1000;
uint16_t PWMModule::pwm_freq = 1000;
uint8_t PWMModule::pwm_cmd_type = 0;

ModuleStatus PWMModule::module_status = ModuleStatus::MODULE_OK;

PWMModule PWMModule::instance = PWMModule();

float PWMModule::cmnd_min = 0;
float PWMModule::cmnd_max = 0;

PWMModule::PWMModule() {
    update_params();
    init();
}

PwmChannelInfo PWMModule::params[static_cast<uint8_t>(PwmPin::PWM_AMOUNT)] = {
    {.pin = PwmPin::PWM_1, .cmd_end_time_ms = 0},  // PWM1
    {.pin = PwmPin::PWM_2, .cmd_end_time_ms = 0},  // PWM2
    {.pin = PwmPin::PWM_3, .cmd_end_time_ms = 0},  // PWM3
    {.pin = PwmPin::PWM_4, .cmd_end_time_ms = 0},  // PWM4
};

PwmChannelsParamsNames
    PWMModule::params_names[static_cast<uint8_t>(PwmPin::PWM_AMOUNT)] = {
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
        PwmPeriphery::set_duration(pwm.pin, (crnt_time_ms < pwm.cmd_end_time_ms)
                                                ? pwm.command_val
                                                : pwm.def);
    }

    status_pub_timeout_ms = 1;
    static uint32_t next_pub_ms = status_pub_timeout_ms;

    if (module_status == ModuleStatus::MODULE_OK && crnt_time_ms > next_pub_ms) {
        publish_state();
        next_pub_ms = crnt_time_ms + status_pub_timeout_ms;
    }
}

void PWMModule::update_params() {
    module_status = ModuleStatus::MODULE_OK;

    pwm_freq = paramsGetIntegerValue(IntParamsIndexes::PARAM_PWM_FREQUENCY);
    pwm_cmd_type = paramsGetIntegerValue(IntParamsIndexes::PARAM_PWM_CMD_TYPE);

    ttl_cmd = paramsGetIntegerValue(IntParamsIndexes::PARAM_PWM_CMD_TTL_MS);
    status_pub_timeout_ms = 100;
    uint8_t max_channel = 0;

    switch (pwm_cmd_type) {
        case 0:
            max_channel = NUMBER_OF_RAW_CMD_CHANNELS - 1;
            break;
        case 1:
            max_channel = 15;
            break;
        default:
            max_channel = 255;
            break;
    }

    bool params_error = false;
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

        auto min = paramsGetIntegerValue(params_names[i].min);
        auto max = paramsGetIntegerValue(params_names[i].max);
        auto def = paramsGetIntegerValue(params_names[i].def);
        if (min <= max && def >= min && def <= max) {
            params[i].min = min;
            params[i].max = max;
            params[i].def = def;
        } else {
            params_error = true;
        }
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
    uint16_t data_type_id = 0;
    uint64_t data_type_signature = 0;

    for (int i = 0; i < static_cast<uint8_t>(PwmPin::PWM_AMOUNT); i++) {
        PwmPeriphery::set_frequency(params[i].pin, pwm_freq);
        switch (pwm_cmd_type) {
            case 0:
                callback = raw_command_callback;
                data_type_signature = UAVCAN_EQUIPMENT_ESC_RAWCOMMAND_SIGNATURE;
                data_type_id = UAVCAN_EQUIPMENT_ESC_RAWCOMMAND_ID;
                publish_state = publish_esc_status;
                cmnd_min = 0.0f;
                cmnd_max = 8191.0f;
                break;

            case 1:
                callback = array_command_callback;
                data_type_signature =
                    UAVCAN_EQUIPMENT_ACTUATOR_ARRAY_COMMAND_SIGNATURE;
                data_type_id = UAVCAN_EQUIPMENT_ACTUATOR_ARRAY_COMMAND_ID;
                publish_state = publish_esc_status;
                cmnd_min = -1.0f;
                cmnd_max = 1.0f;
                // publish_state = publish_actuator_status;
                break;

            default:
                return;
        }
    }
    if (module_status == ModuleStatus::MODULE_OK) {
        uavcanSubscribe(data_type_signature, data_type_id, callback);
    }
}

void PWMModule::publish_esc_status() {
    static uint8_t transfer_id = 0;
    EscStatus_t msg{};
    auto crnt_time_ms = HAL_GetTick();
    static uint32_t next_status_pub_ms[static_cast<uint8_t>(PwmPin::PWM_AMOUNT)];
    for (int i = 0; i < static_cast<uint8_t>(PwmPin::PWM_AMOUNT); i++) {
        auto pwm = params[i];
        if (pwm.channel < 0 || pwm.fb == 0 || next_status_pub_ms[i] > crnt_time_ms) {
            continue;
        }
        msg = {};
        float scaled_value = get_pwm_scaled_value(pwm.pin, -pwm.min, 100.0f / (pwm.max - pwm.min));
        msg.esc_index = pwm.channel;
        msg.power_rating_pct = (uint8_t)(scaled_value);
        if (dronecan_equipment_esc_status_publish(&msg, &transfer_id) == 0) {
            transfer_id++;
            next_status_pub_ms[i] = crnt_time_ms + ((pwm.fb > 1) ? 100 : 1000);
        }
    }
}

float PWMModule::get_pwm_scaled_value(PwmPin pin, int16_t offset, float scale) {
    float pwm_val = (float)(PwmPeriphery::get_duration(pin));
    float scaled_val = (pwm_val + offset) * scale;
    auto val = std::clamp(scaled_val, cmnd_min, cmnd_max);
    return val;
}

// void PWMModule::publish_actuator_status() {
//     static uint8_t transfer_id = 0;
//     ActuatorStatus_t msg {};

//     for (int i =0; i < static_cast<uint8_t>(PwmPin::PWM_AMOUNT); i++) {
//         auto pwm = params[i];
//         if (pwm.channel < 0) continue;
//         auto value = PwmPeriphery::get_duration(pwm.pin);
//         float scaled_value = (value - pwm.min) * 100.0f / (pwm.max -
//         pwm.min); msg.actuator_id = pwm.channel; msg.power_rating_pct =
//         (uint8_t) scaled_value; if
//         (dronecan_equipment_actuator_status_publish(
//                                     &msg, &transfer_id) == 0)
//                                     transfer_id++;
//     }
// }

void PWMModule::raw_command_callback(CanardRxTransfer* transfer) {
    if (module_status != ModuleStatus::MODULE_OK) return;
    RawCommand_t command;

    for (int i = 0; i < static_cast<uint8_t>(PwmPin::PWM_AMOUNT); i++) {
        auto pwm = params[i];
        if (pwm.channel < 0) {
            continue;
        }
        int8_t res = dronecan_equipment_esc_raw_command_channel_deserialize(
            transfer, pwm.channel, &command);
        if (res != 0) {
            continue;
        }
        if (command.raw_cmd[pwm.channel] >= 0) {
            pwm.cmd_end_time_ms = HAL_GetTick() + ttl_cmd;
            pwm.command_val = pwm.min + (pwm.max - pwm.min) *
                                            command.raw_cmd[pwm.channel] /
                                            8191.0;
        } else {
            pwm.command_val = pwm.def;
        }
    }
}

void PWMModule::array_command_callback(CanardRxTransfer* transfer) {
    if (module_status != ModuleStatus::MODULE_OK) return;
    ArrayCommand_t command;
    int8_t ch_num = dronecan_equipment_actuator_arraycommand_deserialize(
        transfer, &command);
    if (ch_num == 0) {
        return;
    }
    for (int i = 0; i < static_cast<uint8_t>(PwmPin::PWM_AMOUNT); i++) {
        auto pwm = params[i];
        if (pwm.channel < 0) {
            continue;
        }
        for (uint8_t i = 0; i < sizeof(command); i++) {
            if (command.commads[i].actuator_id != pwm.channel) {
                continue;
            }
            pwm.cmd_end_time_ms = HAL_GetTick() + ttl_cmd;
            if (command.commads[i].command_value >= 0) {
                pwm.command_val = pwm.min + command.commads[i].command_value *(pwm.max - pwm.min);
            } else {
                pwm.command_val = pwm.def;
            }
        }
    }
}
