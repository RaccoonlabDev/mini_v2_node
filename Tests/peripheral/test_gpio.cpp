/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 */

#include <array>
#include <span>
#include <gtest/gtest.h>
#include "peripheral/gpio/gpio_ubuntu.hpp"

namespace HAL {
namespace {
std::array<bool, 2> kGpios{};
}
std::span<bool> gpios{kGpios};
}  // namespace HAL

class GpioTest : public testing::Test {
protected:
    void SetUp() override {
        HAL::GPIO::set(0, false);
        HAL::GPIO::set(1, false);
    }
};

TEST_F(GpioTest, StoresIndependentPinStates) {
    HAL::GPIO::set(0, true);

    EXPECT_TRUE(HAL::GPIO::get(0));
    EXPECT_FALSE(HAL::GPIO::get(1));

    HAL::GPIO::set(1, true);

    EXPECT_TRUE(HAL::GPIO::get(0));
    EXPECT_TRUE(HAL::GPIO::get(1));
}

TEST_F(GpioTest, InvalidReadReturnsFalse) {
    EXPECT_FALSE(HAL::GPIO::get(HAL::GPIO_INVALID_PIN));
    EXPECT_FALSE(HAL::GPIO::get(HAL::GPIO::pin_count()));
}

TEST_F(GpioTest, InvalidWriteDoesNotChangeValidPins) {
    HAL::GPIO::set(0, true);

    HAL::GPIO::set(HAL::GPIO_INVALID_PIN, false);
    HAL::GPIO::set(HAL::GPIO::pin_count(), false);

    EXPECT_TRUE(HAL::GPIO::get(0));
    EXPECT_FALSE(HAL::GPIO::get(1));
}
