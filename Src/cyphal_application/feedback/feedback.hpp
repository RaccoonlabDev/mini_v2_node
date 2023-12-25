/// This software is distributed under the terms of the MIT License.
/// Copyright (c) 2022-2023 Dmitry Ponomarev.
/// Author: Dmitry Ponomarev <ponomarevda96@gmail.com>

#ifndef SRC_CYPHAL_APPLICATION_FEEDBACK_HPP_
#define SRC_CYPHAL_APPLICATION_FEEDBACK_HPP_

#include "cyphal_publishers.hpp"

#ifdef __cplusplus
extern "C" {
#endif

class FeedbackPublisher: public cyphal::CyphalPublisher {
public:
    FeedbackPublisher(cyphal::Cyphal* driver_) : CyphalPublisher(driver_, 0) {};
    int8_t init();
    void process(uint32_t crnt_time_ms);
    void publish_msg(uint32_t crnt_time_ms);
private:
    uint32_t _prev_pub_ts_ms = 0;
};

#ifdef __cplusplus
}
#endif

#endif  // SRC_CYPHAL_APPLICATION_FEEDBACK_HPP_
