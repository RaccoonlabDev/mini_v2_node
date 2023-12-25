/// This software is distributed under the terms of the MIT License.
/// Copyright (c) 2022-2023 Dmitry Ponomarev.
/// Author: Dmitry Ponomarev <ponomarevda96@gmail.com>

#ifndef SRC_CYPHAL_APPLICATION_SETPOINT_SETPOINT_HPP_
#define SRC_CYPHAL_APPLICATION_SETPOINT_SETPOINT_HPP_

#ifdef __cplusplus
extern "C" {
#endif

#include "cyphal_subscribers.hpp"

class SetpointSubscriber: public cyphal::CyphalSubscriber {
public:
    SetpointSubscriber(cyphal::Cyphal* driver);
    int8_t init();
private:
    void callback(const cyphal::CanardRxTransfer& transfer) override;
};

#ifdef __cplusplus
}
#endif

#endif  // SRC_CYPHAL_APPLICATION_SETPOINT_SETPOINT_HPP_
