#include "PWMModule.hpp"

#define CHANNEL(channel) IntParamsIndexes::PARAM_PWM_##channel##_CH
#define MIN(channel) IntParamsIndexes::PARAM_PWM_##channel##_MIN
#define MAX(channel) IntParamsIndexes::PARAM_PWM_##channel##_MAX
#define DEF(channel) IntParamsIndexes::PARAM_PWM_##channel##_DEF
#define FB(channel) IntParamsIndexes::PARAM_PWM_##channel##_FB

Logger PWMModule::logger = Logger("PWMModule");

uint16_t PWMModule::ttl_cmd = 0;
uint16_t PWMModule::pwm_freq = 1000;
uint8_t PWMModule::pwm_cmd_type = 0;

ModuleStatus PWMModule::module_status = ModuleStatus::MODULE_OK;

PWMModule PWMModule::instance = PWMModule();

PWMModule::PWMModule() {
    update_params();
    init();
}

PwmChannelInfo PWMModule::params[static_cast<uint8_t>(PwmPin::PWM_AMOUNT)] = {
    {.pin = PwmPin::PWM_1},  // PWM1
    {.pin = PwmPin::PWM_2},  // PWM2
    {.pin = PwmPin::PWM_3},  // PWM3
    {.pin = PwmPin::PWM_4},  // PWM4
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
    uavcanSubscribe(UAVCAN_EQUIPMENT_ESC_RAWCOMMAND_SIGNATURE, UAVCAN_EQUIPMENT_ESC_RAWCOMMAND_ID,
                                                                            raw_command_callback);
    uavcanSubscribe(UAVCAN_EQUIPMENT_ACTUATOR_ARRAY_COMMAND_SIGNATURE,
                                                        UAVCAN_EQUIPMENT_ACTUATOR_ARRAY_COMMAND_ID,
                                                        array_command_callback);
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

    if (module_status == ModuleStatus::MODULE_OK &&
        crnt_time_ms > next_pub_ms) {
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

    bool params_error = false;
    switch (pwm_cmd_type) {
        case 0:
            max_channel = NUMBER_OF_RAW_CMD_CHANNELS - 1;
            break;
        case 1:
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

        auto min = paramsGetIntegerValue(params_names[i].min);
        auto max = paramsGetIntegerValue(params_names[i].max);
        auto def = paramsGetIntegerValue(params_names[i].def);
        params[i].def = def;
        params[i].min = min;
        params[i].max = max;
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
        switch (pwm_cmd_type) {
            case 0:
                publish_state = publish_esc_status;
                break;

            case 1:
                publish_state = publish_actuator_status;
                break;

            default:
                return;
        }
    }
}

void PWMModule::publish_esc_status() {
    static uint8_t transfer_id = 0;
    EscStatus_t msg{};
    auto crnt_time_ms = HAL_GetTick();
    static uint32_t
        next_status_pub_ms[static_cast<uint8_t>(PwmPin::PWM_AMOUNT)];
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
    static uint32_t
        next_status_pub_ms[static_cast<uint8_t>(PwmPin::PWM_AMOUNT)];
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

void PWMModule::raw_command_callback(CanardRxTransfer* transfer) {
    if (module_status != ModuleStatus::MODULE_OK || pwm_cmd_type != 0) return;
    RawCommand_t command;
    int8_t ch_num =
        dronecan_equipment_esc_raw_command_deserialize(transfer, &command);
    if (ch_num <= 0) {
        return;
    }
    for (int i = 0; i < static_cast<uint8_t>(PwmPin::PWM_AMOUNT); i++) {
        auto pwm = &params[i];
        if (pwm->channel < 0) {
            continue;
        }
        if (command.raw_cmd[pwm->channel] >= 0) {
            pwm->cmd_end_time_ms = HAL_GetTick() + ttl_cmd;
            pwm->command_val = mapRawCommandToPwm(command.raw_cmd[pwm->channel],
                                                  pwm->min, pwm->max, pwm->def);
        } else {
            pwm->command_val = pwm->def;
        }
    }
}

void PWMModule::array_command_callback(CanardRxTransfer* transfer) {
    if (module_status != ModuleStatus::MODULE_OK || pwm_cmd_type != 1) return;
    ArrayCommand_t command;
    int8_t ch_num = dronecan_equipment_actuator_arraycommand_deserialize(
        transfer, &command);
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
