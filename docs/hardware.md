# Hardware Versioning Standard

## 1 Introduction

Each board and its release **must be versioned** to allow unambiguous identification.

## 2 Version format

The version number follows the `Global.Local.Sub` format:
- **Global version** – the major hardware iteration, synchronized across all boards:
  - **v0** – Legacy
  - **v1** – First iteration: STM32F103, 1× CAN connector, any LED
  - **v2** – Second iteration: STM32F103, 2× CAN connectors, RGB LED
  - **v3** – Third iteration: STM32G0, black boards, 2× CAN FD connectors
- **Local version** – release/order/commit number within a board type.
  Incremented when changes require firmware modifications (e.g., pinout or logic).
  Starts from `0`.
- **Local subversion** – minor revision.
  Incremented when changes do **not** require firmware modifications (e.g., resistor value, layout, LED color).
  Starts from `0`.

Examples:
- `v2.3.1` -> second global iteration (stm32f103), third firmware-affecting release, first cosmetic/PCB-only tweak.
- `v3.1.0` - third global iteration (stm32g0), first firmware-affecting release

## 3 Identification

The version:
- **must be labeled** directly on the PCB
- **should be identified** via a unique resistor network producing a specific voltage on pin **PB1**.

> Note: Firmware can only determine the first two version digits. The third digit is not relevant for software logic.

## 4 Iterations

### v1 – First Iteration (deprecated, STM32F103)
- Identical to v2 except:  
  - No standardized ADC pin for version detection.  
  - Uses a simple LED instead of RGB.  
- **Not recommended** for new designs.

---

### v2 – Second Iteration (STM32F103)
- **Actively used.**  
- Standardized pinout:

#### Common pinout
| Pin  | Function          |
|------|------------------|
| PA0  | ADC_VIN          |
| PA1  | ADC_5V           |
| PA3  | ADC_CURRENT      |
| PA4  | RGB LED – Blue   |
| PA8  | RGB LED – Green  |
| PA15 | RGB LED – Red    |
| PB1  | VERSION          |

#### PWM pinout
| Code/Board Label | Pin  | Timer     |
|------------------|------|----------|
| pwm\<1..N\>      | PB7  | TIM4_CH2 |
| pwm\<1..N\>      | PB6  | TIM4_CH1 |
| pwm\<1..N\>      | PB4  | TIM3_CH1 |
| pwm\<1..N\>      | PB5  | TIM3_CH2 |
| pwm\<1..N\>      | PB3  | TIM2_CH2 |
| pwm\<1..N\>      | PB0  | TIM3_CH3 |

---

### v3 – Third Iteration (STM32G0)
- Standardized pinout (mandatory for all v3 boards):

#### Common pinout
| Pin  | Name                | Function                                  |
|------|---------------------|-------------------------------------------|
| PA0  | ADC_VIN             | Input DC-DC voltage (5–30 V)              |
| PA1  | ADC_5V              | MCU supply voltage (5 V)                  |
| PA6  | ADC_CURRENT         | Current sensor                            |
| PA7  | ADC_VERSION         | Hardware version code                     |
| PC13 | INTERNAL_LED_RED    | Internal RGB LED – Red                    |
| PC14 | INTERNAL_LED_GREEN  | Internal RGB LED – Green                  |
| PC15 | INTERNAL_LED_BLUE   | Internal RGB LED – Blue                   |
| PA15 | CAN1_TERMINATOR     | 120 Ω CAN1 bus termination resistor       |
| PB15 | CAN2_TERMINATOR     | 120 Ω CAN2 bus termination resistor       |

#### External RGB LED pinout
| Pin  | Name                |
|------|---------------------|
| PB9  | EXT_RGB_LED_RED     |
| PB8  | EXT_RGB_LED_GREEN   |
| PB5  | EXT_RGB_LED_BLUE    |

#### PWM pinout
Each PWM pin has **two identifiers**:  
- **User name** – visible in parameters and silkscreen (numbered starting from 1).  
- **Dev name** – internal reference (timer/port/pin), defined in code.

| Pin  | User name | Dev name        |
|------|-----------|-----------------|
| PB9  | pwm\<1..N\> | TIM4_CH4      |
| PB8  | pwm\<1..N\> | TIM4_CH3      |
| PB5  | pwm\<1..N\> | TIM3_CH2      |
| PB4  | pwm\<1..N\> | TIM3_CH1      |
| PB3  | pwm\<1..N\> | TIM2_CH2      |
| PC7  | pwm\<1..N\> | TIM2_CH4      |
| PC6  | pwm\<1..N\> | TIM2_CH3      |
