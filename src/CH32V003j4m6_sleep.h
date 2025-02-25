#ifndef CH32V003j4m6_SLEEP_H
#define CH32V003j4m6_SLEEP_H

#include "ch32v003fun.h"
#include <stddef.h>
#include <stdbool.h>

// Sleep modes
typedef enum {
    SLEEP_MODE_TIMER = 0,    // Wake up after specified time
    SLEEP_MODE_INTERRUPT,    // Wake up on external interrupt only
    SLEEP_MODE_BOTH         // Wake up on either timer or interrupt
} SleepMode;

// Wake-up sources
typedef enum {
    WAKEUP_TIMER = 0,      // Woke up from timer
    WAKEUP_INTERRUPT,      // Woke up from external interrupt
    WAKEUP_UNKNOWN        // Unknown wake-up source
} WakeupSource;

// Sleep states
typedef enum {
    SLEEP_OK = 0,          // Sleep operation successful
    SLEEP_NO_INT_PIN,      // No interrupt pin configured
    SLEEP_INVALID_PIN,     // Invalid pin specified
    SLEEP_TOO_SHORT,       // Sleep time too short
    SLEEP_ERROR           // General error
} SleepStatus;

// Pin definitions
typedef enum {
    NO_PIN = 0,
    WAKE_PA2 = ((0 << 4) | 2),  // Port A (0), Pin 2
    WAKE_PC1 = ((2 << 4) | 1),  // Port C (2), Pin 1
    WAKE_PC2 = ((2 << 4) | 2),  // Port C (2), Pin 2
    WAKE_PC4 = ((2 << 4) | 4),  // Port C (2), Pin 4
    WAKE_PD4 = ((3 << 4) | 4),  // Port D (3), Pin 4
    WAKE_PD6 = ((3 << 4) | 6)   // Port D (3), Pin 6
} WakePin;

// Function declarations
SleepStatus sleep_init(WakePin pin);
SleepStatus sleep_for(uint32_t seconds, SleepMode mode);
WakeupSource get_wakeup_source(void);
SleepStatus set_wake_pin(WakePin pin);

#endif
