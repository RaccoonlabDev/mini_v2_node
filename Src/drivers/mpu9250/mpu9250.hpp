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

enum class FIFOEnableBitmask : uint8_t {
    ACCEL = 0x08,
    GYRO_X = 0x40,
    GYRO_Y = 0x20,
    GYRO_Z = 0x10,
    TEMPERATURE = 0x80,
    ENABLE_ALL = 0xF8
};
constexpr uint8_t operator&(FIFOEnableBitmask lhs, FIFOEnableBitmask rhs) {
    return static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs);
}
using SensorAccel = std::array<int16_t, 3>;
using SensorGyro = std::array<int16_t, 3>;
using SensorMag = std::array<int16_t, 3>;

class Mpu9250 {
public:
    Mpu9250() = default;
    /**
     * @brief Check if the device is connected by reading the WHO_AM_I register
     * @return true on success, false otherwise
     */
    bool initialize(bool useFifo);

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
     * @brief It's programmer responsobility to ensure that FIFO is deactivated and FIFO_EN register is cleared
     * Note: FIFO_reset() ensures that FIFO is deactivated and cleares FIFO_EN register
     * @param bitmask is TEMP_FIFO_EN GYRO_XOUT GYRO_YOUT GYRO_ZOUT ACCEL SLV2 SLV1 SLV0 (8 bits)
     */

    int8_t FIFO_set_resolution (FIFOEnableBitmask set_bitmask);
    /**
     * @return 0 on success, negative otherwise
     * @brief Don't reinitialise FIFO. Reset comes from setting reset bit to 1.
     * If you want to continue use FIFO use  FIFO_init() AND FIFO_set_resolution() after.
     * As example see FIFO_read() in overflow case or FIFO_create()
     */
    int8_t FIFO_reset ();

    /**
     * @return 0 on success, negative otherwise
     */
    int8_t FIFO_init ();

    /**
     * @return 0 on success, negative otherwise
     * @brief It's programmer's responsobility to understand which readings are turned ON and OFF
     */
    int8_t FIFO_read (int16_t* raw_temperature, std::array<int16_t, 3>*  raw_gyro,
        std::array<int16_t, 3>*  raw_accel);

private:
    bool initialized{false};
    uint8_t fifo_frame_bytes = 0;
    const std::byte MPU9250_WHO_AM_I_ID = std::byte{0x71};   // REG_WHO_AM_I expected value

    // Register Map for Gyroscope and Accelerometer
    enum class AccelGyroRegisters : uint8_t {
        ACCEL_XOUT_H = 0x3B,
        GYRO_XOUT_H = 0x43
    };

    enum class TemperatureRegister : uint8_t {
        TEMP_OUT_H = 0x41
    };

    enum class Mpu9250Registers : uint8_t {
        USER_CTRL = 0x6A,
        PWR_MGMT_1 = 0x6B,
        WHO_AM_I = 0x75,
        FIFO_ENABLE = 0x23,
        INT_STATUS = 0x3A,
        FIFO_COUNT_H = 0x72,
        FIFO_R_W = 0x74,
        CONFIG = 0x1A,
        SMPLRT_DIV = 0x19,
        ACCEL_CONFIG2 = 0x1D
    };

    // Register Map for Magnetometer (AK8963)
    enum class MagnetometerResgiter : uint8_t {
        REG_MAG_XOUT_L = 0x03,
    };

    enum class MpuSampleRate : uint8_t {
        SMAPLE_RATE_500HZ = 0x01,
        SAMPLE_RATE_250HZ = 0x03,
        SAMPLE_RATE_200HZ = 0x04,
        SAMPLE_RATE_125HZ = 0x07,
        SAMPLE_RATE_100HZ = 0x09,
        SAMPLE_RATE_50HZ = 0x13
    };
    // Default: enable reading for all
    FIFOEnableBitmask bitmask = FIFOEnableBitmask::ENABLE_ALL;

    /**
     * @return amout of bytes in FIFO
     */
    uint16_t FIFO_count ();

    /**
     * @return 0 on success, negative otherwise
     * @brief Activate FIFO in imu for further usage
     */
    int8_t FIFO_create ();
};

#endif  // SRC_DRIVERS_MPU9250_MPU9250_HPP_
