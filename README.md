# ch32v003j4m6 Deep Sleep Library

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform: CH32V003](https://img.shields.io/badge/Platform-CH32V003-blue.svg)](http://www.wch-ic.com/products/CH32V003.html)

A simple and efficient deep sleep management library for ch32v003j4m6 microcontrollers. This library provides easy-to-use functions for managing deep sleep modes with timer and/or interrupt wake-up capabilities.

## Features

- Timer-based wake-up
- External interrupt wake-up (configurable pin)
- Combined timer and interrupt wake-up
- Configurable sleep duration
- Wake-up source detection
- Low power consumption
- Simple API

## Installation

### PlatformIO

1. Create a `lib` folder in your project if it doesn't exist
2. Clone this repository into the `lib` folder:
```bash
cd lib
git clone https://github.com/your-username/ch32v003j4m6-sleep.git
```

### Manual Installation

1. Copy the `ch32v003j4m6_sleep.h` and `ch32v003j4m6_sleep.c` files to your project
2. Include the header file in your code: `#include "ch32v003j4m6_sleep.h"`

## Usage

### Basic Example

```c
#include "ch32v003j4m6_sleep.h"

void main() {
    // Initialize sleep with PA2 as wake-up pin
    sleep_init(WAKE_PA2);

    // Sleep for 60 seconds, wake on either timer or PA2 interrupt
    SleepStatus status = sleep_for(60, SLEEP_MODE_BOTH);

    if (status == SLEEP_OK) {
        switch (get_wakeup_source()) {
            case WAKEUP_TIMER:
                // Woke up after timeout
                break;
            case WAKEUP_INTERRUPT:
                // Woke up from button press
                break;
        }
    }
}
```

### Sleep Modes

The library supports three sleep modes:

1. **Timer Only (SLEEP_MODE_TIMER)**
   ```c
   // Sleep for 50 seconds, wake up only after timeout
   sleep_for(50, SLEEP_MODE_TIMER);
   ```

2. **Interrupt Only (SLEEP_MODE_INTERRUPT)**
   ```c
   // Sleep indefinitely, wake up only on interrupt
   sleep_for(0, SLEEP_MODE_INTERRUPT);
   ```

3. **Both Timer and Interrupt (SLEEP_MODE_BOTH)**
   ```c
   // Sleep for 60 seconds, wake up on either timeout or interrupt
   sleep_for(60, SLEEP_MODE_BOTH);
   ```

### Available Wake-up Pins

The following pins can be configured as wake-up sources:

- `WAKE_PA2` (Pin 3)
- `WAKE_PC1` (Pin 5)
- `WAKE_PC2` (Pin 6)
- `WAKE_PC4` (Pin 7)
- `WAKE_PD4` (Pin 8)
- `WAKE_PD6` (Pin 1)

### Error Handling

The library provides comprehensive error handling:

```c
SleepStatus status = sleep_for(60, SLEEP_MODE_BOTH);
switch (status) {
    case SLEEP_OK:
        // Operation successful
        break;
    case SLEEP_NO_INT_PIN:
        // No interrupt pin configured
        break;
    case SLEEP_INVALID_PIN:
        // Invalid pin specified
        break;
    case SLEEP_TOO_SHORT:
        // Sleep time too short
        break;
    case SLEEP_ERROR:
        // General error
        break;
}
```

## API Reference

### Functions

#### `sleep_init(WakePin pin)`
Initializes the sleep functionality with optional wake-up pin.
- Parameters:
  - `pin`: Wake-up pin (use NO_PIN for timer-only operation)
- Returns: `SleepStatus`

#### `sleep_for(uint32_t seconds, SleepMode mode)`
Enters sleep mode for specified duration.
- Parameters:
  - `seconds`: Sleep duration in seconds (0 for interrupt-only mode)
  - `mode`: Sleep mode (TIMER, INTERRUPT, or BOTH)
- Returns: `SleepStatus`

#### `get_wakeup_source()`
Gets the source that caused wake-up.
- Returns: `WakeupSource`

#### `set_wake_pin(WakePin pin)`
Configures a new wake-up pin after initialization.
- Parameters:
  - `pin`: New wake-up pin
- Returns: `SleepStatus`

## Hardware Configuration

The library uses:
- Auto-Wake-Up (AWU) feature with LSI oscillator
- External interrupt on configured pin
- Power-down mode for minimal power consumption

## Power Consumption

In deep sleep mode, the ch32v003j4m6 typically consumes:
- ~2.5µA with LSI running (timer mode)
- ~1.5µA in interrupt-only mode

## License

This library is released under the MIT License. See the LICENSE file for details.

## Author

[Shakir Abdo](https://github.com/shakir-abdo)

