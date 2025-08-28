#ifndef DEEPSLEEP_H
#define DEEPSLEEP_H

#include "ch32v003fun.h"

// Wake-up method enumeration
typedef enum {
    DEEPSLEEP_WAKE_TIMER = 0,
    DEEPSLEEP_WAKE_EXTERNAL_INT
} deepsleep_wake_method_t;

// Wake-up pin enumeration for CH32V003J4M6
typedef enum {
    DEEPSLEEP_PIN_PD6 = 0,  // PD6
    DEEPSLEEP_PIN_PA2,      // PA2
    DEEPSLEEP_PIN_PC2,      // PC2
    DEEPSLEEP_PIN_PC1,      // PC1
    DEEPSLEEP_PIN_PC4       // PC4
} deepsleep_wake_pin_t;

// Interrupt trigger type
typedef enum {
    DEEPSLEEP_TRIGGER_FALLING = 0,
    DEEPSLEEP_TRIGGER_RISING,
    DEEPSLEEP_TRIGGER_BOTH
} deepsleep_trigger_t;

// Configuration structure
typedef struct {
    deepsleep_wake_method_t wake_method;
    union {
        struct {
            uint32_t seconds;
        } timer;
        struct {
            deepsleep_wake_pin_t pin;
            deepsleep_trigger_t trigger;
        } external_int;
    } config;
} deepsleep_config_t;

// API Functions
void deepsleep_init(void);
void deepsleep_config_timer(uint32_t seconds);
void deepsleep_config_external_int(deepsleep_wake_pin_t pin, deepsleep_trigger_t trigger);
void deepsleep_enter(void);
uint8_t deepsleep_is_awake(void);
void deepsleep_reset_wake_flag(void);

#endif // DEEPSLEEP_H