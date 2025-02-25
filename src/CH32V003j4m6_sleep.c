
#include "CH32V003j4m6_sleep.h"

#include <stdbool.h>  // for bool type
#include <stddef.h>   // for NULL

// Constants
#define LSI_FREQ 128000UL
#define AWU_PRESCALER 4096UL
#define MAX_AWU_COUNT 63U
#define TIME_PER_COUNT ((float)AWU_PRESCALER / LSI_FREQ)
#define MAX_SINGLE_SLEEP (MAX_AWU_COUNT * TIME_PER_COUNT)

// Static variables
static bool initialized = false;
static WakePin current_wake_pin = NO_PIN;
static volatile WakeupSource last_wake_source = WAKEUP_UNKNOWN;

// Forward declarations
static void setup_pin(WakePin pin);
static GPIO_TypeDef* get_pin_port(WakePin pin);
static uint8_t get_pin_number(WakePin pin);
static SleepStatus sleep_one_cycle(uint8_t counts, SleepMode mode);

// Interrupt handlers
void EXTI7_0_IRQHandler(void) __attribute__((interrupt));
void AWU_IRQHandler(void) __attribute__((interrupt));

void EXTI7_0_IRQHandler(void) {
    last_wake_source = WAKEUP_INTERRUPT;
    EXTI->INTFR = (1 << get_pin_number(current_wake_pin));
}

void AWU_IRQHandler(void) {
    last_wake_source = WAKEUP_TIMER;
    EXTI->INTFR = EXTI_Line9;
}

SleepStatus sleep_init(WakePin pin) {
    if (initialized) return SLEEP_OK;

    // Enable power interface clock
    RCC->APB1PCENR |= RCC_APB1Periph_PWR;

    // Enable LSI oscillator
    RCC->RSTSCKR |= RCC_LSION;
    while ((RCC->RSTSCKR & RCC_LSIRDY) == 0) {
    }

    // Configure AWU
    EXTI->INTENR |= EXTI_Line9;
    EXTI->FTENR |= EXTI_Line9;

    // Configure AWU prescaler
    PWR->AWUPSC |= PWR_AWU_Prescaler_4096;

    // Select standby mode
    PWR->CTLR |= PWR_CTLR_PDDS;

    // Enable AWU interrupt
    NVIC_EnableIRQ(AWU_IRQn);

    initialized = true;

    // Configure wake-up pin if specified
    if (pin != NO_PIN) {
        return set_wake_pin(pin);
    }

    return SLEEP_OK;
}

SleepStatus set_wake_pin(WakePin pin) {
    if (!initialized) return SLEEP_ERROR;

    if (get_pin_port(pin) == NULL) return SLEEP_INVALID_PIN;

    current_wake_pin = pin;
    setup_pin(pin);

    return SLEEP_OK;
}

SleepStatus sleep_for(uint32_t seconds, SleepMode mode) {
    if (!initialized) return SLEEP_ERROR;

    if ((mode == SLEEP_MODE_INTERRUPT || mode == SLEEP_MODE_BOTH) &&
        current_wake_pin == NO_PIN) {
        return SLEEP_NO_INT_PIN;
    }

    if (seconds == 0 && mode != SLEEP_MODE_INTERRUPT) {
        return SLEEP_TOO_SHORT;
    }

    if (mode == SLEEP_MODE_INTERRUPT) {
        return sleep_one_cycle(0, mode);
    }

    uint32_t remaining_seconds = seconds;
    while (remaining_seconds > 0) {
        uint32_t sleep_time;
        uint8_t counts;

        if (remaining_seconds >= 2) {
            counts = MAX_AWU_COUNT;
            sleep_time = 2;
        } else {
            counts = (uint8_t)(remaining_seconds / TIME_PER_COUNT);
            if (counts == 0) counts = 1;
            sleep_time = 1;
        }

        SleepStatus status = sleep_one_cycle(counts, mode);
        if (status != SLEEP_OK) return status;

        if (last_wake_source == WAKEUP_INTERRUPT && mode != SLEEP_MODE_BOTH) {
            break;
        }

        remaining_seconds -= sleep_time;
    }

    return SLEEP_OK;
}

WakeupSource get_wakeup_source(void) { return last_wake_source; }

static void setup_pin(WakePin pin) {
    GPIO_TypeDef* port = get_pin_port(pin);
    uint8_t pin_num = get_pin_number(pin);
    uint8_t port_num = (((uint8_t)pin) >> 4) & 0x0F;

    // Enable GPIO clock
    RCC->APB2PCENR |= (1 << (port_num + 2));

    // Configure pin as input with pull-up
    port->CFGLR &= ~(0xf << (4 * pin_num));
    port->CFGLR |= (GPIO_CNF_IN_PUPD << (4 * pin_num));
    port->BSHR = (1 << pin_num);

    // Enable AFIO clock
    RCC->APB2PCENR |= RCC_APB2Periph_AFIO;

    // Configure AFIO EXTI mapping
    uint32_t temp = AFIO->EXTICR;
    temp &= ~(0x0F << ((pin_num & 0x03) * 4));
    temp |= port_num << ((pin_num & 0x03) * 4);
    AFIO->EXTICR = temp;

    // Enable EXTI line
    EXTI->INTENR |= (1 << pin_num);
    EXTI->FTENR |= (1 << pin_num);

    // Enable NVIC interrupt
    NVIC_EnableIRQ(EXTI7_0_IRQn);
}

static GPIO_TypeDef* get_pin_port(WakePin pin) {
    uint8_t port_num = (((uint8_t)pin) >> 4) & 0x0F;
    switch (port_num) {
        case 0:
            return GPIOA;
        case 2:
            return GPIOC;
        case 3:
            return GPIOD;
        default:
            return NULL;
    }
}

static uint8_t get_pin_number(WakePin pin) { return ((uint8_t)pin) & 0x0F; }

static SleepStatus sleep_one_cycle(uint8_t counts, SleepMode mode) {
    last_wake_source = WAKEUP_UNKNOWN;

    if (mode == SLEEP_MODE_TIMER || mode == SLEEP_MODE_BOTH) {
        PWR->AWUWR &= ~0x3f;
        PWR->AWUWR |= counts;
        PWR->AWUCSR |= (1 << 1);
    }

    // Clear pending interrupts
    EXTI->INTFR = EXTI_Line9;
    if (current_wake_pin != NO_PIN) {
        EXTI->INTFR = (1 << get_pin_number(current_wake_pin));
    }

    __WFI();

    SystemInit();

    return SLEEP_OK;
}
