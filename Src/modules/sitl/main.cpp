/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include <cinttypes>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
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

static bool read_host_cpu_celsius(float* celsius) {
    FILE* file = std::fopen("/sys/class/thermal/thermal_zone0/temp", "r");
    if (file == nullptr) {
        return false;
    }

    int32_t millidegrees = 0;
    const bool is_parsed = std::fscanf(file, "%" SCNd32, &millidegrees) == 1;
    std::fclose(file);
    if (!is_parsed || millidegrees < -40000 || millidegrees > 125000) {
        return false;
    }

    *celsius = static_cast<float>(millidegrees) / 1000.0f;
    return true;
}

// The MCU temperature of the simulated board: SITL_TEMPERATURE_C if it is set, else the host CPU
// temperature, else about 25 °C.
static float board_celsius(float time_s) {
    if (const char* forced = std::getenv("SITL_TEMPERATURE_C"); forced != nullptr) {
        return std::strtof(forced, nullptr);
    }

    float celsius;
    if (read_host_cpu_celsius(&celsius)) {
        return celsius;
    }

    return 25.0f + std::sin(time_s * 0.6f + 1.2f);
}

// On SITL BoardMonitor::temperature() uses the STM32F103 formula T = (1750 - adc) / 5 + 25 °C.
static int32_t celsius_to_adc(float celsius) {
    return 1750 - static_cast<int32_t>(std::lround((celsius - 25.0f) * 5.0f));
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
        const auto temperature = celsius_to_adc(board_celsius(t));

        set_adc_if_present(BoardAdc::RANK_VIN, vin);
        set_adc_if_present(BoardAdc::RANK_5V, v5);
        set_adc_if_present(BoardAdc::RANK_CURRENT, current);
        set_adc_if_present(BoardAdc::RANK_VERSION, version);
        set_adc_if_present(BoardAdc::RANK_TEMPERATURE, temperature);
    }
};

REGISTER_MODULE(SitlModule)

}  // namespace
