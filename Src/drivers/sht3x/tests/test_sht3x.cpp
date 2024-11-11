/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include <gtest/gtest.h>
#include "drivers/sht3x/sht3x.hpp"

TEST(Shx3xTest, HelloWorld) {
    EXPECT_EQ(1, 1);
}

TEST(Shx3xTest, init) {
    Driver::SHT3X sht3x(Driver::SHT3X::DEV_ADDR_PIN_LOW);
}

TEST(Shx3xTest, read) {
    Driver::SHT3X sht3x(Driver::SHT3X::DEV_ADDR_PIN_LOW);

    float temperature;
    float humidity;
    sht3x.read(&temperature, &humidity);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
