/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#ifndef SRC_DRIVERS_MPU9250_MPU9250_HPP_
#define SRC_DRIVERS_MPU9250_MPU9250_HPP_

#include <cstdint>
#include <array>
#include "common/logging.hpp"
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

    /**
     * @return 0 on success, negative otherwise
     */
    int8_t read_temperature (int16_t& temperature) const;

    /**
     * @return 0 on success, negative otherwise
     */
    int8_t FIFO_reset ();

    /**
     * @return 0 on success, negative otherwise
     */
    int8_t FIFO_init ();

    /**
     * @return 0 on success, negative otherwise
     * @brief it's programmer responsobility to provide correct data which correspond to given bitmask in set_resolution
     * Note: By default bitmask is 0b11111000
     */
    int8_t FIFO_read (int16_t* raw_temperature, std::array<int16_t, 3>*  raw_gyro, std::array<int16_t, 3>*  raw_accel);
    /**
     * @param bitmask is TEMP_FIFO_EN GYRO_XOUT GYRO_YOUT GYRO_ZOUT ACCEL SLV2 SLV1 SLV0 (8 bits)
     * @return 0 on success, negative otherwise
     */

    uint16_t FIFO_count ();
private:
    bool initialized{false};
    uint8_t fifo_entities = 0;
    // Default: enable reading for all
    std::byte bitmask {0b11111000};
};

#endif  // SRC_DRIVERS_MPU9250_MPU9250_HPP_
