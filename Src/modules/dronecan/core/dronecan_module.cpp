/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "dronecan_module.hpp"
#include <algorithm>
#include "params.hpp"
#include "libdcnode/dronecan.h"
#include "libdcnode/can_driver.h"
#include "peripheral/adc/circuit_periphery.hpp"

#ifndef GIT_HASH
    #warning "GIT_HASH has been assigned to 0 by default."
    #define GIT_HASH            (uint64_t)0
#endif

#ifndef APP_VERSION_MAJOR
    #warning "APP_VERSION_MAJOR has been assigned to 0 by default."
    #define APP_VERSION_MAJOR   0  // NOSONAR: fallback for missing CMake version macro
#endif

#ifndef APP_VERSION_MINOR
    #warning "APP_VERSION_MINOR has been assigned to 0 by default."
    #define APP_VERSION_MINOR   0  // NOSONAR: fallback for missing CMake version macro
#endif

#ifndef HW_VERSION_MAJOR
    #warning "HW_VERSION_MAJOR has been assigned to 0 by default."
    #define HW_VERSION_MAJOR    0  // NOSONAR: fallback for missing CMake version macro
#endif

#ifndef HW_VERSION_MINOR
    #warning "HW_VERSION_MINOR has been assigned to 0 by default."
    #define HW_VERSION_MINOR    0  // NOSONAR: fallback for missing CMake version macro
#endif

#ifdef __cplusplus
extern "C" {
#endif
uint32_t platformSpecificGetTimeMs();
bool platformSpecificRequestRestart();
void platformSpecificReadUniqueID(uint8_t out_uid[4]);
#ifdef __cplusplus
}
#endif

REGISTER_MODULE(DronecanModule)

DronecanModule::DronecanModule() : Module(0, Protocol::DRONECAN) {
}

static bool paramsIsInteger(ParamIndex_t param_idx) {
    return paramsGetType(param_idx) == PARAM_TYPE_INTEGER;
}
static bool paramsIsString(ParamIndex_t param_idx) {
    return paramsGetType(param_idx) == PARAM_TYPE_STRING;
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

void DronecanModule::init() {
    ParamsApi params_api = {
        .getName = paramsGetName,
        .isInteger = paramsIsInteger,
        .isString = paramsIsString,
        .find = paramsFind,
        .save = paramsSave,
        .resetToDefault = paramsResetToDefault,

        .integer = {
            .setValue = paramsSetIntegerValue,
            .getValue = paramsGetIntegerValue,
            .getMin = paramsGetIntegerMin,
            .getMax = paramsGetIntegerMax,
            .getDef = paramsGetIntegerDef,
        },

        .string = {
            .setValue = paramsSetStringValue,
            .getValue = paramsGetStringValue,
        },
    };

    PlatformApi platform_api{
        .getTimeMs = platformSpecificGetTimeMs,
        .requestRestart = platformSpecificRequestRestart,
        .readUniqueId = platformSpecificReadUniqueID,
        .can = {
            .init = canDriverInit,
            .recv = canDriverReceive,
            .send = canDriverTransmit,
            .getRxOverflowCount = canDriverGetRxOverflowCount,
            .getErrorCount = canDriverGetErrorCount,
        }
    };

    auto node_name_param_idx = static_cast<ParamIndex_t>(StrParamsIndexes::PARAM_SYSTEM_NAME);
    const auto current_node_name = (const char*)paramsGetStringValue(node_name_param_idx);
    if(strlen(current_node_name) == 0) {
        const auto& [board_name, name_length] = CircuitPeriphery::get_board_name();
        paramsSetStringValue(node_name_param_idx, name_length, (const uint8_t*)board_name);
    }
    int param_node_id_value = paramsGetIntegerValue(IntParamsIndexes::PARAM_UAVCAN_NODE_ID);
    AppInfo app_info{
        .node_id = static_cast<uint8_t>(std::clamp(param_node_id_value, 1, 126)),
        .node_name = (const char*)paramsGetStringValue(node_name_param_idx),
        .vcs_commit = GIT_HASH >> 32,
        .sw_version_major = APP_VERSION_MAJOR,
        .sw_version_minor = APP_VERSION_MINOR,
        .hw_version_major = HW_VERSION_MAJOR,
        .hw_version_minor = HW_VERSION_MINOR,
    };

    int8_t res = uavcanInitApplication(params_api, platform_api, &app_info);

    set_health(res >= 0 ? Status::OK : Status::FATAL_MALFANCTION);
    set_mode(Mode::STANDBY);
}

void DronecanModule::spin_once() {
    uavcanSetNodeHealth(static_cast<NodeStatusHealth_t>(ModuleManager::get_global_status()));
    auto global_mode = (uint8_t)(ModuleManager::get_global_mode());
    if (global_mode > 0 ) {
        global_mode -= 1;
    }
    uavcanSetNodeStatusMode(static_cast<NodeStatusMode_t>(global_mode));
    uavcanSetVendorSpecificStatusCode(ModuleManager::get_vssc());
    uavcanSpinOnce();
}
