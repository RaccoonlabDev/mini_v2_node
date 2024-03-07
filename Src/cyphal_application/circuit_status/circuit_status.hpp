/// This software is distributed under the terms of the MIT License.
/// Copyright (c) 2024 Dmitry Ponomarev.
/// Author: Dmitry Ponomarev <ponomarevda96@gmail.com>

#ifndef SRC_CYPHAL_APPLICATION_CIRCUIT_STATUS_CIRCUIT_STATUS_HPP_
#define SRC_CYPHAL_APPLICATION_CIRCUIT_STATUS_CIRCUIT_STATUS_HPP_

#include "cyphal_publishers.hpp"
#include "Udral/circuit_status.hpp"

#ifdef __cplusplus
extern "C" {
#endif

class CircuitStatus {
public:
    /**
     * @brief The constructor must be called only after initialization of Cyphal instance!
     */
    CircuitStatus() = default;

    /**
     * @brief This function should be called once in the beginning of the application
     * @return >= 0 on success and < 0 on error
     */
    int8_t init() const;

    /**
     * @brief This function should be called periodically. It should automatically:
     * - udpate parameters,
     * - estimate public and internal states
     * - publish everyting to CAN 
     */
    void process(uint32_t crnt_time_ms);

private:
    void _spin_once();
    void _update_parameters();

    RaccoonLab::CircuitStatusVoltagePublisher voltage_5v_pub{
        cyphal::Cyphal::get_instance(), 65535};

    RaccoonLab::CircuitStatusVoltagePublisher voltage_vin_pub{
        cyphal::Cyphal::get_instance(), 65535};

    RaccoonLab::CircuitStatusTemperaturePublisher temperature_pub{
        cyphal::Cyphal::get_instance(), 65535};

    uint32_t _prev_pub_ts_ms = 0;
};

#ifdef __cplusplus
}
#endif

#endif  // SRC_CYPHAL_APPLICATION_CIRCUIT_STATUS_CIRCUIT_STATUS_HPP_
