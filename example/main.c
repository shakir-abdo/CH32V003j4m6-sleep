#include "ch32v003fun.h"
#include "CH32V003j4m6_sleep.h"

void main() {
    // Initialize system clock
    SystemInit();
    // Initialize sleep with PA2 as wake-up pin
    sleep_init(WAKE_PA2);

    // Sleep for 60 seconds, wake on either timer or PA2 interrupt
    SleepStatus status = sleep_for(60, SLEEP_MODE_BOTH);

    // Check wake-up source
    if (status == SLEEP_OK) {
        switch (get_wakeup_source()) {
            case WAKEUP_TIMER:
                // Woke up after timeout
                break;
            case WAKEUP_INTERRUPT:
                // Woke up from button press
                break;
            default:
                // Unknown wake-up source
                break;
        }
    }

}
