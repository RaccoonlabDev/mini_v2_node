/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include <cmath>
#include <cstdint>
#include "module.hpp"
#include "peripheral/adc/adc.hpp"
#include "adc_mapping.hpp"

namespace {

static uint16_t clamp_12bit(int32_t value) {
    if (value < 0) {
        return 0;
    }
    if (value > 4095) {
        return 4095;
    }
    return static_cast<uint16_t>(value);
}

static void set_adc_if_present(uint8_t rank, int32_t value) {
    if (rank == BoardAdc::INVALID_RANK) {
        return;
    }
    HAL::Adc::set(rank, clamp_12bit(value));
}

class SitlModule : public Module {
public:
    SitlModule() : Module(20.0f, Protocol::CYPHAL_AND_DRONECAN) {}

    void init() override {
        Module::init();
        set_mode(Mode::ENGAGED);
    }

protected:
    void spin_once() override {
        const float t = static_cast<float>(HAL_GetTick()) / 1000.0f;

        const auto vin = 640 + static_cast<int32_t>(20.0f * std::sin(t * 2.0f));
        const auto v5 = 3200 + static_cast<int32_t>(12.0f * std::sin(t * 1.3f + 0.9f));
        const auto current = 2048 + static_cast<int32_t>(25.0f * std::sin(t * 1.7f + 2.1f));
        const auto version = 3848;
        const auto temperature = 3115 + static_cast<int32_t>(6.0f * std::sin(t * 0.6f + 1.2f));

        set_adc_if_present(BoardAdc::RANK_VIN, vin);
        set_adc_if_present(BoardAdc::RANK_5V, v5);
        set_adc_if_present(BoardAdc::RANK_CURRENT, current);
        set_adc_if_present(BoardAdc::RANK_VERSION, version);
        set_adc_if_present(BoardAdc::RANK_TEMPERATURE, temperature);
    }
};

REGISTER_MODULE(SitlModule)

}  // namespace
