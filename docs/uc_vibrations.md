# USE CASE - VIBRATIONS ANALYSIS

## 1. SUMMARY

Use the onboard IMU to publish raw motion plus vibration metrics (level and dominant frequency) over DroneCAN, enabling quick diagnostics and trending without extra sensors.

## 2. SETUP

No additional hardware is required beyond the onboard MPU-9250. This use case works out of the box on supported nodes.

## 3. PARAMETERS (ESSENTIAL ONLY)

| Param             | Values   | Meaning |
| ----------------- | -------- | ------- |
| imu.publishers    | [0; 15]  | Bit mask for IMU publishers: </br> Bit 1 - RawImu publisher </br> Bit 2 - vibration metric in RawImu.integration_interval </br> Bit 3 - FFT accel in RawImu.accelerometer_integral </br> Bit 4 - FFT gyro in RawImu.rate_gyro_integral |
| imu.pub_frequency | [0; 256] | Publish rate [Hz]. 0 disables RawImu publishing. |

## 4. PUBLISHERS / SUBSCRIBERS (ESSENTIAL ONLY)

**Publishers (outgoing):**

DroneCAN doesn't provide a dedicated vibration message, so the standard `ahrs.RawIMU` is reused.

| Data type | Notes |
| --------- | ----- |
| [ahrs.RawIMU](https://dronecan.github.io/Specification/7._List_of_standard_data_types/#rawimu) | `timestamp` is global network-synchronized time, </br> `rate_gyro_latest` is the latest gyro measurement, </br> `accelerometer_latest` is the latest accel measurement. </br> `integration_interval` is the accel vibration metric (PX4-style), </br> `rate_gyro_integral` is `[dom freq, mag, 0]` for gyro, </br> `accelerometer_integral` is `[dom freq, mag, 0]` for accel. |

¹ `timestamp` is always 0 because global network-synchronized time is not supported yet

## 5. USAGE

**1. Raw IMU measurement**

The simplest thing is to use a raw imu measurements.

<img src="https://raw.githubusercontent.com/wiki/RaccoonlabDev/mini_v2_node/assets/raw_imu_gui_tool.png" alt="drawing" width="900">

**2. Vibrations level measurement**

Vibrations metric is calculated as norm of difference beetween norm of measurements processed in moving average filter.

| How it is implemented in PX4 | How it can be analysed with gui_tool |
|-|-|
| <img src="https://raw.githubusercontent.com/wiki/RaccoonlabDev/mini_v2_node/assets/vibrations_px4.png" alt="drawing" width="390"> | <img src="https://raw.githubusercontent.com/wiki/RaccoonlabDev/mini_v2_node/assets/vibrations_gui_tool.png" alt="drawing" width="510"> |

**3. Dominant frequency and magnitude measurement**

The dominant frequency and the magnitude are calculated with FFT algorithm.

| PX4 flight review provides </br> Acceleration Power Spectral Density | We provide only a dominant frequency and a magnitude |
|-|-|
| <img src="https://raw.githubusercontent.com/wiki/RaccoonlabDev/mini_v2_node/assets/acceleration_power_spectral_density_px4.png" alt="drawing" width="390"> | ... |

## 6. NOTES

### 6.1 Algorithm details (FFT)

Requirements:
- ICE rotation speed is usually less than: 12000 RPM (200 Hz),
- Motor rotation speed is usually less than 6000 RPM (100 Hz).

So, MPU9250 should measure accelerometer data with 400 Hz rate to capture all frequencies.

```c++
constexpr size_t NUMBER_OF_SAMPLES = 400;
```

**Step 1. Collecting samples**  
Each sample is a magnitude:

```c++
std::array<float, 3> raw_accel_sample = {{...}};
float magnitude = std::sqrt(std::pow(raw_accel_sample[0], 2) + std::pow(raw_accel_sample[1], 2) + std::pow(raw_accel_sample[2], 2));
```

After 400 samples:

```c++
std::array<float, NUMBER_OF_SAMPLES> accel_magnitudes;
```

**Step 2. Applying windowing**  
Use a Hanning window to reduce spectral leakage:

$$
w(n) = 0.5 \times \left( 1 - \cos\left( \frac{2\pi n}{N-1} \right) \right)
$$

```c++
std::array<float, NUMBER_OF_SAMPLES> windowed_buffer;
```

Alternatives: Rectangular (no windowing), Hamming, Blackman (better leakage reduction, higher cost).

**Step 3. Applying FFT**  
Convert time-domain signal into the frequency domain.

**Step 4. Finding peaks**  
Extract dominant frequency and magnitude.

### 6.2 References

- STM32: CMSIS-DSP
- Ubuntu: fftw3
