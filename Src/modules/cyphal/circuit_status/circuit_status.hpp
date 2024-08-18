/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_CYPHAL_APPLICATION_CIRCUIT_STATUS_CIRCUIT_STATUS_HPP_
#define SRC_CYPHAL_APPLICATION_CIRCUIT_STATUS_CIRCUIT_STATUS_HPP_

#include "cyphal_publishers.hpp"
#include "Udral/circuit_status.hpp"
#include "periphery/adc/circuit_periphery.hpp"
#include "module.hpp"

#ifdef __cplusplus
extern "C" {
#endif

class CircuitStatus : public Module {
public:
    CircuitStatus() : Module(2, Protocol::CYPHAL) {}
    void init() override;

protected:
    void update_params() override;
    void spin_once() override;

private:
    RaccoonLab::CircuitStatusVoltagePublisher voltage_5v_pub{
        cyphal::Cyphal::get_instance(), 65535};

    RaccoonLab::CircuitStatusVoltagePublisher voltage_vin_pub{
        cyphal::Cyphal::get_instance(), 65535};

    RaccoonLab::CircuitStatusTemperaturePublisher temperature_pub{
        cyphal::Cyphal::get_instance(), 65535};
};

#ifdef __cplusplus
}
#endif

#endif  // SRC_CYPHAL_APPLICATION_CIRCUIT_STATUS_CIRCUIT_STATUS_HPP_
