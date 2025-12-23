/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_CYPHAL_APPLICATION_CIRCUIT_STATUS_CIRCUIT_STATUS_HPP_
#define SRC_CYPHAL_APPLICATION_CIRCUIT_STATUS_CIRCUIT_STATUS_HPP_

#include "libcpnode/cyphal.hpp"
#include "module.hpp"
#include "uavcan/si/unit/temperature/Scalar_1_0.h"
#include "uavcan/si/unit/voltage/Scalar_1_0.h"

#ifdef __cplusplus
extern "C" {
#endif

class CyphalCircuitStatus : public Module {
public:
    CyphalCircuitStatus() : Module(2, Protocol::CYPHAL) {}
    void init() override;

protected:
    void update_params() override;
    void spin_once() override;

private:
    libcpnode::Cyphal::Publisher<libcpnode::SubjectTraits<uavcan_si_unit_voltage_Scalar_1_0>> voltage_5v_pub;
    libcpnode::Cyphal::Publisher<libcpnode::SubjectTraits<uavcan_si_unit_voltage_Scalar_1_0>> voltage_vin_pub;
    libcpnode::Cyphal::Publisher<libcpnode::SubjectTraits<uavcan_si_unit_temperature_Scalar_1_0>> temperature_pub;
};

#ifdef __cplusplus
}
#endif

#endif  // SRC_CYPHAL_APPLICATION_CIRCUIT_STATUS_CIRCUIT_STATUS_HPP_
