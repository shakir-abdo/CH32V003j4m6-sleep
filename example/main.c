/*
 * DeepSleep Library Example
 * 
 * This example demonstrates how to use the DeepSleep library for CH32V003J4M6
 * microcontroller with both timer and external interrupt wake-up methods.
 * 
 * Hardware Setup:
 * - LED on PC4 (for visual indication)
 * - Button/switch on PC1 (for external interrupt wake-up)
 * 
 * The example alternates between:
 * 1. Timer wake-up mode (10 seconds)
 * 2. External interrupt wake-up mode (button press on PC1)
 */

#include "ch32v003fun.h"
#include "deepsleep.h"

#define LED_PIN 4
#define LED_PORT GPIOC

// Global flag to track which mode we're using
uint8_t use_timer_mode = 1;

void gpio_init(void) {
    // Enable GPIOC clock
    RCC->APB2PCENR |= RCC_APB2Periph_GPIOC;
    
    // Configure PC4 as push-pull output for LED
    GPIOC->CFGLR &= ~(0xf << (4 * 4));
    GPIOC->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP) << (4 * 4);
}

void led_on(void) {
    LED_PORT->BSHR = (1 << LED_PIN);
}

void led_off(void) {
    LED_PORT->BCR = (1 << LED_PIN);
}

void blink_pattern(uint8_t count) {
    for (uint8_t i = 0; i < count; i++) {
        led_on();
        Delay_Ms(150);
        led_off();
        Delay_Ms(150);
    }
}

int main() {
    SystemInit();
    gpio_init();
    
    // Initialize deepsleep library
    deepsleep_init();
    
    // Initial startup indication (5 fast blinks)
    blink_pattern(5);
    Delay_Ms(1000);
    
    while(1) {
        if (use_timer_mode) {
            // Timer Mode Example
            // Configure timer wake-up for 10 seconds
            deepsleep_config_timer(10);
            
            // Indicate timer mode with 2 blinks
            blink_pattern(2);
            Delay_Ms(500);
            
            // Enter deep sleep for 10 seconds
            deepsleep_enter();
            
            // MCU wakes up here after 10 seconds
            // Switch to interrupt mode for next cycle
            use_timer_mode = 0;
            
        } else {
            // External Interrupt Mode Example
            // Configure external interrupt on PC1 with falling edge trigger
            deepsleep_config_external_int(DEEPSLEEP_PIN_PC1, DEEPSLEEP_TRIGGER_FALLING);
            
            // Indicate interrupt mode with 3 blinks
            blink_pattern(3);
            Delay_Ms(500);
            
            // Enter deep sleep until PC1 interrupt (button press)
            deepsleep_enter();
            
            // MCU wakes up here when PC1 goes low (button pressed)
            // Switch back to timer mode for next cycle
            use_timer_mode = 1;
        }
        
        // Wake-up indication (1 long blink)
        led_on();
        Delay_Ms(1000);
        led_off();
        Delay_Ms(500);
    }
    
    return 0;
}

/*
 * Usage Instructions:
 * 
 * 1. Upload this code to your CH32V003J4M6
 * 2. Connect an LED to PC4 (with current limiting resistor)
 * 3. Connect a button between PC1 and GND (pull-up is enabled internally)
 * 
 * Expected Behavior:
 * - 5 fast blinks at startup
 * - Timer mode: 2 blinks → sleep for 10 seconds → wake up (1 long blink)
 * - Interrupt mode: 3 blinks → sleep until button press → wake up (1 long blink)
 * - Alternates between timer and interrupt modes
 * 
 * Power Consumption:
 * - Active: ~1-3mA (during blinks and delays)
 * - Sleep (timer): ~10µA
 * - Sleep (interrupt): ~5µA
 */