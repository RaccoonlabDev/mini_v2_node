/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "common.hpp"
#include "periphery/adc/circuit_periphery.hpp"
#include "periphery/led/led.hpp"
#include "params.hpp"

int8_t init_board_periphery() {
    LedPeriphery::reset();
    CircuitPeriphery::init();

    paramsInit((ParamIndex_t)IntParamsIndexes::INTEGER_PARAMS_AMOUNT, NUM_OF_STR_PARAMS, -1, 1);
    paramsLoad();

    return 0;
}

uint8_t get_node_id() {
    return paramsGetIntegerValue(IntParamsIndexes::PARAM_UAVCAN_NODE_ID);
}
