/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_CYPHAL_APPLICATION_FEEDBACK_HPP_
#define SRC_CYPHAL_APPLICATION_FEEDBACK_HPP_

#include "cyphal_publishers.hpp"

#ifdef __cplusplus
extern "C" {
#endif

class FeedbackPublisher: public cyphal::CyphalPublisher {
public:
    explicit FeedbackPublisher(cyphal::Cyphal* driver_) : CyphalPublisher(driver_, 0) {}
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
