# DeepSleep Library for CH32V003J4M6

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform: CH32V003](https://img.shields.io/badge/Platform-CH32V003-blue.svg)](http://www.wch-ic.com/products/CH32V003.html)

A simple and efficient deep sleep management library for CH32V003J4M6 microcontrollers using the ch32v003fun framework. This library provides easy-to-use functions for managing deep sleep modes with timer and external interrupt wake-up capabilities.

## Features

- Timer-based wake-up (configurable seconds)
- External interrupt wake-up on any available pin
- Support for all CH32V003J4M6 pins: PD6, PA2, PC2, PC1, PC4
- Configurable interrupt triggers: falling, rising, or both edges
- Low power consumption using sleep mode
- Simple and intuitive API
- Built for ch32v003fun framework

## Installation

### PlatformIO

1. Copy the `deepsleep` folder to your project's `lib/` directory
2. Include the header file in your code: `#include "deepsleep.h"`

## Usage

### Timer-Based Wake-up Example

```c
#include "ch32v003fun.h"
#include "deepsleep.h"

int main() {
    SystemInit();
    
    // Initialize deepsleep library
    deepsleep_init();
    
    // Configure timer wake-up for 60 seconds
    deepsleep_config_timer(60);
    
    while(1) {
        // Your active code here
        
        // Enter deep sleep for 60 seconds
        deepsleep_enter();
        
        // MCU wakes up here after 60 seconds
    }
    
    return 0;
}
```

### External Interrupt Wake-up Example

```c
#include "ch32v003fun.h"
#include "deepsleep.h"

int main() {
    SystemInit();
    
    // Initialize deepsleep library
    deepsleep_init();
    
    // Configure external interrupt on PC1 with falling edge trigger
    deepsleep_config_external_int(DEEPSLEEP_PIN_PC1, DEEPSLEEP_TRIGGER_FALLING);
    
    while(1) {
        // Your active code here
        
        // Enter deep sleep until PC1 interrupt
        deepsleep_enter();
        
        // MCU wakes up here when PC1 goes low
    }
    
    return 0;
}
```

### Available Wake-up Pins

The CH32V003J4M6 supports the following pins for external interrupt wake-up:

- `DEEPSLEEP_PIN_PD6` - Port D, Pin 6
- `DEEPSLEEP_PIN_PA2` - Port A, Pin 2
- `DEEPSLEEP_PIN_PC2` - Port C, Pin 2
- `DEEPSLEEP_PIN_PC1` - Port C, Pin 1
- `DEEPSLEEP_PIN_PC4` - Port C, Pin 4

### Interrupt Trigger Types

- `DEEPSLEEP_TRIGGER_FALLING` - Trigger on falling edge (high to low)
- `DEEPSLEEP_TRIGGER_RISING` - Trigger on rising edge (low to high)
- `DEEPSLEEP_TRIGGER_BOTH` - Trigger on both edges

## API Reference

### Functions

#### `void deepsleep_init(void)`
Initializes the deepsleep library. Must be called before any other deepsleep functions.

#### `void deepsleep_config_timer(uint32_t seconds)`
Configures timer-based wake-up.
- **Parameters:**
  - `seconds`: Sleep duration in seconds (1-65535)
- **Example:** `deepsleep_config_timer(30);` // Sleep for 30 seconds

#### `void deepsleep_config_external_int(deepsleep_wake_pin_t pin, deepsleep_trigger_t trigger)`
Configures external interrupt wake-up.
- **Parameters:**
  - `pin`: Wake-up pin (DEEPSLEEP_PIN_PD6, DEEPSLEEP_PIN_PA2, etc.)
  - `trigger`: Trigger type (DEEPSLEEP_TRIGGER_FALLING, DEEPSLEEP_TRIGGER_RISING, DEEPSLEEP_TRIGGER_BOTH)
- **Example:** `deepsleep_config_external_int(DEEPSLEEP_PIN_PC1, DEEPSLEEP_TRIGGER_FALLING);`

#### `void deepsleep_enter(void)`
Enters deep sleep mode. The MCU will wake up based on the configured method (timer or external interrupt).

#### `uint8_t deepsleep_is_awake(void)`
Checks if the MCU has woken up from sleep.
- **Returns:** 1 if awake, 0 if still sleeping

#### `void deepsleep_reset_wake_flag(void)`
Resets the internal wake-up flag.

## Hardware Configuration

The library uses:
- **TIM1** for timer-based wake-up with configurable timeout
- **External interrupts (EXTI)** on specified pins
- **Sleep mode** (not deep power-down) to maintain peripheral functionality
- **Pull-up resistors** automatically enabled on interrupt pins

## Power Consumption

In sleep mode, the CH32V003J4M6 typically consumes:
- **Timer mode:** ~10µA with TIM1 running
- **Interrupt mode:** ~5µA with EXTI configured
- **Active mode:** ~1-3mA depending on clock frequency and peripheral usage

## License

This library is released under the MIT License. See the LICENSE file for details.

## Author

[Shakir Abdo](https://github.com/shakir-abdo)

