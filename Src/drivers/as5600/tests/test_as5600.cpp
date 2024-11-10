/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include <gtest/gtest.h>
#include "drivers/as5600/as5600.hpp"

TEST(AS5600Test, HelloWorld) {
    EXPECT_EQ(1, 1);
}

TEST(AS5600Test, init) {
    Driver::AS5600 as5600;
    as5600.init();
}

TEST(AS5600Test, is_ready) {
    Driver::AS5600 as5600;
    as5600.is_ready();
}

TEST(AS5600Test, get_angle) {
    Driver::AS5600 as5600;
    as5600.get_angle();
}

TEST(AS5600Test, get_magnitude) {
    Driver::AS5600 as5600;
    as5600.get_magnitude();
}

TEST(AS5600Test, get_status) {
    Driver::AS5600 as5600;
    as5600.get_status();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}