#include "PWMModule.hpp"

#define CHANNEL(channel) IntParamsIndexes::PARAM_PWM_##channel##_CH
#define MIN(channel) IntParamsIndexes::PARAM_PWM_##channel##_MIN
#define MAX(channel) IntParamsIndexes::PARAM_PWM_##channel##_MAX
#define DEF(channel) IntParamsIndexes::PARAM_PWM_##channel##_DEF

Logger PWMModule::logger = Logger("LightsModule");

uint8_t PWMModule::node_id = 0;

uint16_t PWMModule::ttl_cmd = 1000;
uint32_t PWMModule::pwm_freq = 1000;
uint8_t PWMModule::pwm_cmd_type = 0;

ModuleStatus PWMModule::module_status = ModuleStatus::MODULE_OK;

PWMModule PWMModule::instance = PWMModule();

PWMModule::PWMModule() {
    update_params();
    init();
}

PwmChannelInfo PWMModule::params[static_cast<uint8_t>(PwmPin::PWM_AMOUNT)] = {
    {.pin = PwmPin::PWM_1, .def = 0, .channel = 0},     // PWM1
    {.pin = PwmPin::PWM_2, .def = 0, .channel = 0},     // PWM2
    {.pin = PwmPin::PWM_3, .def = 0, .channel = 0},     // PWM3
    {.pin = PwmPin::PWM_4, .def = 0, .channel = 0},     // PWM4
};

PwmChannelsParamsNames PWMModule::params_names[static_cast<uint8_t>(PwmPin::PWM_AMOUNT)] = {
    {.min = MIN(1), .max = MAX(1), .def = DEF(1), .ch = CHANNEL(1)},     // PWM1
    {.min = MIN(2), .max = MAX(2), .def = DEF(2), .ch = CHANNEL(2)},     // PWM2
    {.min = MIN(3), .max = MAX(3), .def = DEF(3), .ch = CHANNEL(3)},     // PWM3
    {.min = MIN(4), .max = MAX(4), .def = DEF(4), .ch = CHANNEL(4)},     // PWM4
};

PWMModule &PWMModule::get_instance() {
    instance.update_params();
    instance.init();

    return instance;
}

void PWMModule::init() {
    logger.init("PWMModule");
    update_params();
    for (int i =0; i < static_cast<uint8_t>(PwmPin::PWM_AMOUNT); i++) {

        PwmPeriphery::init(params[i].pin);
    }
    set_params();
}

void PWMModule::spin_once() {
    uint32_t crnt_time_ms = HAL_GetTick();

    static uint32_t next_update_ms = 0;
    if (crnt_time_ms > next_update_ms) {
        next_update_ms = crnt_time_ms + 100;
        update_params();
        set_params();
    }

    for (int i =0; i < static_cast<uint8_t>(PwmPin::PWM_AMOUNT); i++) {
        auto pwm = params[i];
        PwmPeriphery::set_duration(pwm.pin, (crnt_time_ms > pwm.cmd_end_time_ms)? pwm.duration : pwm.def);
    }

    static uint32_t next_pub_ms = 100;
    if (verbose && crnt_time_ms > next_pub_ms && module_status == ModuleStatus::MODULE_OK) {
        publish_state();
        next_pub_ms = crnt_time_ms + 100;
    }
    
}

void PWMModule::update_params() {
    module_status = ModuleStatus::MODULE_OK;

    pwm_freq = paramsGetIntegerValue(IntParamsIndexes::PARAM_PWM_FREQUENCY);
    pwm_cmd_type = paramsGetIntegerValue(IntParamsIndexes::PARAM_PWM_CMD_TYPE);

    ttl_cmd = paramsGetIntegerValue(IntParamsIndexes::PARAM_PWM_CMD_TTL_MS);
    verbose = paramsGetIntegerValue(IntParamsIndexes::PARAM_PWM_VERBOSE);
    node_id = paramsGetIntegerValue(IntParamsIndexes::PARAM_UAVCAN_NODE_ID);

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
    for (int i =0; i < static_cast<uint8_t>(PwmPin::PWM_AMOUNT); i++) {
        uint8_t channel = paramsGetIntegerValue(params_names[i].ch);
        if (channel < max_channel)
            params[i].channel = channel;
        else {
            params[i].channel = max_channel;
            params_error = true;
        }

        auto min = paramsGetIntegerValue(params_names[i].min);
        auto max = paramsGetIntegerValue(params_names[i].max);
        auto def = paramsGetIntegerValue(params_names[i].def);
        if (min > max || def < min || def > max) {
            params[i].min = min;
            params[i].max = max;
            params[i].def = def;
        } else params_error = true;
    }

    if (params_error) {
        module_status = ModuleStatus::MODULE_WARN;
        logger.log_warn("check parameters");
    }

}

void PWMModule::set_params() {
    uint16_t data_type_id = 0;
    uint64_t data_type_signature = 0;

    for (int i =0; i < static_cast<uint8_t>(PwmPin::PWM_AMOUNT); i++) {
        PwmPeriphery::set_frequency(params[i].pin, pwm_freq);
        switch (pwm_cmd_type) {
            case 0:
                callback = raw_command_callback;
                data_type_signature = UAVCAN_EQUIPMENT_ESC_RAWCOMMAND_SIGNATURE;
                data_type_id = UAVCAN_EQUIPMENT_ESC_RAWCOMMAND_ID;
                publish_state = publish_raw_command;
                break;

            case 1:
                callback = array_command_callback;
                data_type_signature = UAVCAN_EQUIPMENT_ACTUATOR_ARRAY_COMMAND_SIGNATURE;
                data_type_id = UAVCAN_EQUIPMENT_ACTUATOR_ARRAY_COMMAND_ID;
                publish_state = publish_array_command;
                break;
            
            default:
                return;
        }
    }
    if (module_status == ModuleStatus::MODULE_OK)
        uavcanSubscribe(data_type_signature, data_type_id, callback);
}

void PWMModule::publish_array_command() {
    static uint8_t array_transfer_id = 0;
    ArrayCommand_t array_cmd{};

    for (int i =0; i < static_cast<uint8_t>(PwmPin::PWM_AMOUNT); i++) {
        auto pwm = params[i];

        auto value = PwmPeriphery::get_duration(pwm.pin);
        float scaled_value = (value - pwm.min) / ((float)(pwm.max - pwm.min));
        array_cmd.commands[pwm.channel].actuator_id = pwm.channel;
        array_cmd.commands[pwm.channel].command_type = 0;
        array_cmd.commands[pwm.channel].command_value = scaled_value;
    }
    if (dronecan_equipment_actuator_arraycommand_publish(
                                &array_cmd, &array_transfer_id) == 0)
                                array_transfer_id ++;
}

void PWMModule::publish_raw_command() {
    static uint8_t raw_transfer_id = 0;
    RawCommand_t raw_cmd {};

    for (int i =0; i < static_cast<uint8_t>(PwmPin::PWM_AMOUNT); i++) {

        auto pwm = params[i];

        auto value = PwmPeriphery::get_duration(pwm.pin);
        float scaled_value = (value - pwm.min) * 8191.0 / ((float)(pwm.max - pwm.min));
        raw_cmd.raw_cmd[pwm.channel] = scaled_value;
    }
    if (dronecan_equipment_esc_raw_command_publish(
                                &raw_cmd, &raw_transfer_id) == 0)
                                raw_transfer_id++;
}

void PWMModule::raw_command_callback(CanardRxTransfer* transfer) {
    if (module_status != ModuleStatus::MODULE_OK) return;
    RawCommand_t command;

    for (int i =0; i < static_cast<uint8_t>(PwmPin::PWM_AMOUNT); i++) {
        auto pwm = params[i];

        int8_t res = dronecan_equipment_esc_raw_command_channel_deserialize(transfer, pwm.channel, &command);
        if (res == 0) {
            pwm.cmd_end_time_ms = HAL_GetTick() + ttl_cmd;
            if (command.raw_cmd[pwm.channel] >= 0)
                pwm.duration = pwm.min + (pwm.max - pwm.min) * command.raw_cmd[pwm.channel]/ 8191.0;
            else pwm.duration = pwm.def;
        }
    }
}

void PWMModule::array_command_callback(CanardRxTransfer* transfer) {
    if (module_status != ModuleStatus::MODULE_OK) return;
    ArrayCommand_t command;
    int8_t res = dronecan_equipment_actuator_arraycommand_deserialize(transfer, &command);
    if (res == 0) {
        for (int i =0; i < static_cast<uint8_t>(PwmPin::PWM_AMOUNT); i++) {
            auto pwm = params[i];

            for (uint8_t i = 0; i < sizeof(command); i++) {
                if (command.commands[i].actuator_id == pwm.channel) {
                    pwm.cmd_end_time_ms = HAL_GetTick() + ttl_cmd;
                    if (command.commands[i].command_value >= 0)
                        pwm.duration = pwm.min + command.commands[i].command_value * (pwm.max - pwm.min);
                    else pwm.duration = pwm.def;
                }
            }
        }
    }
}
