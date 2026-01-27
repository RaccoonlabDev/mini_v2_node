# USE CASE - HYGROMETER (I2C, SHT3X)

## 1. SUMMARY

Read humidity from an SHT3x sensor over I2C and publish it over DroneCAN using the standard hygrometer message for environmental monitoring.

## 2. SETUP

Connect the SHT3x sensor to the I2C pins (PWM1/PWM2 used as SDA/SCL in the CAN‑I2C converter setup). Provide 3.3V/5V and GND according to the sensor board requirements.

## 3. PARAMETERS / REGISTERS (ESSENTIAL ONLY)

No required parameters beyond enabling your I2C/peripheral setup and app logic. Use your application’s register set to select I2C pins and sampling rate if applicable.

## 4. PUBLISHERS / SUBSCRIBERS (ESSENTIAL ONLY)

**Publishers (outgoing):**
| Data type | Notes |
| --------- | ----- |
| `dronecan.sensors.hygrometer.Hygrometer` | Publishes relative humidity (float, percent). |

**Subscribers (incoming):**
| Data type | Notes |
| --------- | ----- |
| — | — |

## 5. USAGE

Initialize the SHT3x driver on the I2C bus and periodically read humidity. Publish it via the DroneCAN hygrometer message and verify values in gui_tool or logs.

## 6. NOTES

- Driver: `Src/drivers/sht3x/sht3x.hpp` and `Src/drivers/sht3x/sht3x.cpp`.
- Message definition: `Libs/Dronecan/include/libdcnode/dronecan/sensors/hygrometer/Hygrometer.h`.
