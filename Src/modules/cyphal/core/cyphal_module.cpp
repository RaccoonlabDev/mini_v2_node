/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "cyphal_module.hpp"
#include <algorithm>
#include "libcpnode/cyphal.hpp"
#include "params.hpp"
#include "peripheral/adc/circuit_periphery.hpp"

REGISTER_MODULE(CyphalModule)

#ifdef __cplusplus
extern "C" {
#endif
uint32_t platformSpecificGetTimeMs();
bool platformSpecificRequestRestart();
void platformSpecificReadUniqueID(uint8_t out_uid[4]);
#ifdef __cplusplus
}
#endif

static bool paramsIsInteger(ParamIndex_t param_idx) {
    return paramsGetType(param_idx) == PARAM_TYPE_INTEGER;
}

static bool paramsIsString(ParamIndex_t param_idx) {
    return paramsGetType(param_idx) == PARAM_TYPE_STRING;
}

static uint8_t paramsGetTypeU8(ParamIndex_t param_idx) {
    return static_cast<uint8_t>(paramsGetType(param_idx));
}

static IntegerParamValue_t paramsGetIntegerMin(ParamIndex_t param_idx) {
    auto desc = paramsGetIntegerDesc(param_idx);
    return desc != nullptr ? desc->min : 0;
}

static IntegerParamValue_t paramsGetIntegerMax(ParamIndex_t param_idx) {
    auto desc = paramsGetIntegerDesc(param_idx);
    return desc != nullptr ? desc->max : 0;
}

static IntegerParamValue_t paramsGetIntegerDef(ParamIndex_t param_idx) {
    auto desc = paramsGetIntegerDesc(param_idx);
    return desc != nullptr ? desc->def : 0;
}

static bool paramsIsMutable(ParamIndex_t param_idx) {
    auto param_type = paramsGetType(param_idx);
    if (param_type == PARAM_TYPE_INTEGER) {
        auto desc = paramsGetIntegerDesc(param_idx);
        return desc != nullptr && desc->is_mutable;
    }
    if (param_type == PARAM_TYPE_STRING) {
        auto desc = paramsGetStringDesc(param_idx);
        return desc != nullptr && desc->is_mutable;
    }
    return false;
}

libcpnode::PlatformApi buildCyphalPlatformApi() {
    libcpnode::PlatformApi api{};

    api.getName = paramsGetName;
    api.isInteger = paramsIsInteger;
    api.isString = paramsIsString;
    api.getType = paramsGetTypeU8;
    api.find = paramsFind;
    api.isMutable = paramsIsMutable;
    api.save = paramsSave;
    api.resetToDefault = paramsResetToDefault;

    api.integer.setValue = paramsSetIntegerValue;
    api.integer.getValue = paramsGetIntegerValue;
    api.integer.getMin = paramsGetIntegerMin;
    api.integer.getMax = paramsGetIntegerMax;
    api.integer.getDef = paramsGetIntegerDef;

    api.string.setValue = paramsSetStringValue;
    api.string.getValue = paramsGetStringValue;

    api.getTimeMs = platformSpecificGetTimeMs;
    api.requestRestart = platformSpecificRequestRestart;
    api.readUniqueId = platformSpecificReadUniqueID;

    return api;
}

libcpnode::AppInfo buildCyphalAppInfo() {
    auto node_name_param_idx = static_cast<ParamIndex_t>(StrParamsIndexes::PARAM_SYSTEM_NAME);
    const auto current_node_name = (const char*)paramsGetStringValue(node_name_param_idx);
    if (current_node_name == nullptr || strlen(current_node_name) == 0) {
        const auto& [board_name, name_length] = CircuitPeriphery::get_board_name();
        paramsSetStringValue(node_name_param_idx, name_length, (const uint8_t*)board_name);
    }

    int param_node_id_value = paramsGetIntegerValue(IntParamsIndexes::PARAM_UAVCAN_NODE_ID);
    libcpnode::AppInfo app_info{
        .node_id = static_cast<uint8_t>(std::clamp(param_node_id_value, 1, 126)),
        .node_name = (const char*)paramsGetStringValue(node_name_param_idx),
        .vcs_commit = GIT_HASH >> 32,
        .sw_version_major = APP_VERSION_MAJOR,
        .sw_version_minor = APP_VERSION_MINOR,
        .hw_version_major = HW_VERSION_MAJOR,
        .hw_version_minor = HW_VERSION_MINOR,
    };

    return app_info;
}

CyphalModule::CyphalModule()
    : Module(0, Protocol::CYPHAL),
      cyphal(buildCyphalPlatformApi(), buildCyphalAppInfo()) {
}

void CyphalModule::init() {
    auto node_name_param_idx = static_cast<ParamIndex_t>(IntParamsIndexes::INTEGER_PARAMS_AMOUNT);
    const auto& [board_name, name_length] = CircuitPeriphery::get_board_name();
    paramsSetStringValue(node_name_param_idx, name_length, (const uint8_t*)board_name);

    int param_node_id_value = paramsGetIntegerValue(IntParamsIndexes::PARAM_UAVCAN_NODE_ID);
    auto node_id = std::clamp(param_node_id_value, 1, 126);

    int8_t res = cyphal.init(node_id);

    libcpnode::NodeGetInfoSubscriber::setHardwareVersion(2, 1);

    set_health(res >= 0 ? Status::OK : Status::FATAL_MALFANCTION);
    set_mode(Mode::STANDBY);
}

void CyphalModule::spin_once() {
    cyphal.setNodeHealth(uavcan_node_Health_1_0{(uint8_t)(ModuleManager::get_global_status())});
    auto global_mode = (uint8_t)(ModuleManager::get_global_mode());
    if (global_mode > 0 ) {
        global_mode -= 1;
    }
    cyphal.setNodeMode(uavcan_node_Mode_1_0{global_mode});
    cyphal.setVSSC(ModuleManager::get_vssc());
    cyphal.process();
}
