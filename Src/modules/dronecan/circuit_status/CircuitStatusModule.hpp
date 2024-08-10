/***
 * Copyright (C) 2024 Anastasiia Stepanova  <asiiapine@gmail.com>
 *  Distributed under the terms of the GPL v3 license, available in the file LICENSE.
***/ 

#ifndef SRC_MODULES_CIRCUIT_STATUS_CIRCUIT_STATUS_HPP_
#define SRC_MODULES_CIRCUIT_STATUS_CIRCUIT_STATUS_HPP_

#include "common/module.hpp"
#include "publisher.hpp"

class CircuitStatus : public Module {
public:
    CircuitStatus() : Module(2.0f) {}
    void init() override;

protected:
    void spin_once() override;

private:
    DronecanPublisher<CircuitStatus_t> circuit_status_pub;
    DronecanPublisher<Temperature_t> dev_temperature_pub;
};

#endif  // SRC_MODULES_CIRCUIT_STATUS_CIRCUIT_STATUS_HPP_
