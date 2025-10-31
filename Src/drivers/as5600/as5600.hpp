/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Anastasiia Stepanova <asiiapine@gmail.com>
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */
#ifndef SRC_DRIVERS_AS5600_AS5600_HPP_
#define SRC_DRIVERS_AS5600_AS5600_HPP_

#include <stdint.h>

namespace Driver {

class AS5600 {
public:
    /**
     * @brief Verify that the device is avaliable via I2C and status is ok
     * @return 0 on success, negative error code otherwise
     */
    static int8_t init();

    /**
     * @return True if the device is ready for measurement, False otherwise
     */
    static bool is_ready();

    /**
     * @brief The RAW ANGLE register contains the unscaled and unmodified angle.
     * @return Angle within [0, 360] on success, negative error code otherwise
     */
    static int16_t get_angle();

    /**
     * @brief The MAGNITUDE register indicates the magnitude value of the internal CORDIC.
     * @return Unitless magnitude within [0, 4095] on success, negative error code otherwise
     */
    static int16_t get_magnitude();

    /**
     * @brief Read the STATUS register
     * This register provides bits that indicate the current state of the AS5600:
     * MH (4) - AGC minimum gain overflow, magnet too strong
     * ML (5) - AGC maximum gain overflow, magnet too weak
     * MD (6) - Magnet was detected
     * @return Register register value on success, negative error code otherwise
     */
    static int16_t get_status();

    static constexpr const char* STATUS_MH = "AGC minimum gain overflow, magnet too strong";
    static constexpr const char* STATUS_ML = "AGC maximum gain overflow, magnet too weak";
    static constexpr const char* STATUS_MD = "Magnet was detected";

    /**
     * @note AS5600 registers
     */
    static constexpr uint8_t REG_RAW_ANGLE = 0x0C;  // Unscaled and unmodified angle
    static constexpr uint8_t REG_ANGLE = 0x0E;      // Scaled output value
    static constexpr uint8_t REG_STATUS = 0x0B;     // Bits that indicate the current state
    static constexpr uint8_t REG_MAGNITUDE = 0x1B;  // The magnitude value of the internal CORDIC

    static constexpr uint8_t I2C_ADDRESS_AS5600 = 0x36;
    static constexpr uint8_t I2C_AS5600 = (I2C_ADDRESS_AS5600 << 1) + 1;
};

}  // namespace Driver

#endif  // SRC_DRIVERS_AS5600_AS5600_HPP_
