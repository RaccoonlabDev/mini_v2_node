/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_DRIVERS_MPU9250_MPU9250_HPP_
#define SRC_DRIVERS_MPU9250_MPU9250_HPP_

#include <cstdint>
#include <array>

class Mpu9250 {
public:
    Mpu9250() = default;

    /**
     * @brief Check if the device is connected by reading the WHO_AM_I register
     * @return true on success, false otherwise
     */
    bool initialize();

    /**
     * @return 0 on success, negative otherwise
     */
    int8_t read_accelerometer(std::array<int16_t, 3>* accel) const;

    /**
     * @return 0 on success, negative otherwise
     */
    int8_t read_gyroscope(std::array<int16_t, 3>* gyro) const;

    /**
     * @return 0 on success, negative otherwise
     */
    int8_t read_magnetometer(std::array<int16_t, 3>* mag) const;

    int8_t set_accel_odr(uint8_t odr);

    int8_t set_gyro_odr(uint8_t odr);
private:
    bool initialized{false};
};

#endif  // SRC_DRIVERS_MPU9250_MPU9250_HPP_
