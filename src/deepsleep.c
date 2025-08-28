#include "deepsleep.h"

// Private variables
static deepsleep_config_t deepsleep_cfg;
static volatile uint8_t wake_up_flag = 0;

// Timer interrupt handler
void TIM1_UP_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM1_UP_IRQHandler(void) {
    if (TIM1->INTFR & TIM_UIF) {
        TIM1->INTFR = ~TIM_UIF;  // Clear interrupt flag
        wake_up_flag = 1;
        TIM1->CTLR1 &= ~TIM_CEN;  // Disable timer
    }
}

// External interrupt handlers
void EXTI7_0_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void EXTI7_0_IRQHandler(void) {
    // Handle EXTI0-7 interrupts (PA2, PC1, PC2, PC4)
    if (EXTI->INTFR & EXTI_Line1) {  // PA2 (Line 1)
        EXTI->INTFR = EXTI_Line1;
        wake_up_flag = 1;
    }
    if (EXTI->INTFR & EXTI_Line2) {  // PC2 (Line 2)
        EXTI->INTFR = EXTI_Line2;
        wake_up_flag = 1;
    }
    if (EXTI->INTFR & EXTI_Line1) {  // PC1 (Line 1)
        EXTI->INTFR = EXTI_Line1;
        wake_up_flag = 1;
    }
    if (EXTI->INTFR & EXTI_Line4) {  // PC4 (Line 4)
        EXTI->INTFR = EXTI_Line4;
        wake_up_flag = 1;
    }
}

// Initialize deepsleep library
void deepsleep_init(void) {
    wake_up_flag = 0;
    deepsleep_cfg.wake_method = DEEPSLEEP_WAKE_TIMER;
    deepsleep_cfg.config.timer.seconds = 10;  // Default 10 seconds
}

// Configure timer wake-up
void deepsleep_config_timer(uint32_t seconds) {
    deepsleep_cfg.wake_method = DEEPSLEEP_WAKE_TIMER;
    deepsleep_cfg.config.timer.seconds = seconds;
    
    // Enable TIM1 clock
    RCC->APB2PCENR |= RCC_APB2Periph_TIM1;
    
    // Configure TIM1 for specified timeout
    // Assuming 48MHz system clock, prescaler = 48000-1 gives 1kHz timer frequency
    TIM1->PSC = 48000 - 1;  // Prescaler for 1kHz (1ms per tick)
    TIM1->ATRLR = (seconds * 1000) - 1;  // Auto-reload for specified seconds
    
    // Enable update interrupt
    TIM1->DMAINTENR |= TIM_UIE;
    
    // Enable TIM1 interrupt in NVIC
    NVIC_EnableIRQ(TIM1_UP_IRQn);
}

// Configure external interrupt wake-up
void deepsleep_config_external_int(deepsleep_wake_pin_t pin, deepsleep_trigger_t trigger) {
    deepsleep_cfg.wake_method = DEEPSLEEP_WAKE_EXTERNAL_INT;
    deepsleep_cfg.config.external_int.pin = pin;
    deepsleep_cfg.config.external_int.trigger = trigger;
    
    // Enable AFIO and GPIO clocks
    RCC->APB2PCENR |= RCC_APB2Periph_AFIO;
    
    switch (pin) {
        case DEEPSLEEP_PIN_PD6:
            // Enable GPIOD clock
            RCC->APB2PCENR |= RCC_APB2Periph_GPIOD;
            // Configure PD6 as input with pull-up
            GPIOD->CFGLR &= ~(0xf << (6 * 4));
            GPIOD->CFGLR |= (GPIO_CNF_IN_PUPD) << (6 * 4);
            GPIOD->BSHR = (1 << 6);  // Enable pull-up
            // Configure EXTI line 6
            AFIO->EXTICR |= AFIO_EXTICR_EXTI6_PD;
            break;
            
        case DEEPSLEEP_PIN_PA2:
            // Enable GPIOA clock
            RCC->APB2PCENR |= RCC_APB2Periph_GPIOA;
            // Configure PA2 as input with pull-up
            GPIOA->CFGLR &= ~(0xf << (2 * 4));
            GPIOA->CFGLR |= (GPIO_CNF_IN_PUPD) << (2 * 4);
            GPIOA->BSHR = (1 << 2);  // Enable pull-up
            // Configure EXTI line 2
            AFIO->EXTICR |= AFIO_EXTICR_EXTI2_PA;
            break;
            
        case DEEPSLEEP_PIN_PC2:
            // Enable GPIOC clock
            RCC->APB2PCENR |= RCC_APB2Periph_GPIOC;
            // Configure PC2 as input with pull-up
            GPIOC->CFGLR &= ~(0xf << (2 * 4));
            GPIOC->CFGLR |= (GPIO_CNF_IN_PUPD) << (2 * 4);
            GPIOC->BSHR = (1 << 2);  // Enable pull-up
            // Configure EXTI line 2
            AFIO->EXTICR |= AFIO_EXTICR_EXTI2_PC;
            break;
            
        case DEEPSLEEP_PIN_PC1:
            // Enable GPIOC clock
            RCC->APB2PCENR |= RCC_APB2Periph_GPIOC;
            // Configure PC1 as input with pull-up
            GPIOC->CFGLR &= ~(0xf << (1 * 4));
            GPIOC->CFGLR |= (GPIO_CNF_IN_PUPD) << (1 * 4);
            GPIOC->BSHR = (1 << 1);  // Enable pull-up
            // Configure EXTI line 1
            AFIO->EXTICR |= AFIO_EXTICR_EXTI1_PC;
            break;
            
        case DEEPSLEEP_PIN_PC4:
            // Enable GPIOC clock
            RCC->APB2PCENR |= RCC_APB2Periph_GPIOC;
            // Configure PC4 as input with pull-up
            GPIOC->CFGLR &= ~(0xf << (4 * 4));
            GPIOC->CFGLR |= (GPIO_CNF_IN_PUPD) << (4 * 4);
            GPIOC->BSHR = (1 << 4);  // Enable pull-up
            // Configure EXTI line 4
            AFIO->EXTICR |= AFIO_EXTICR_EXTI4_PC;
            break;
    }
    
    // Configure interrupt trigger
    uint32_t exti_line = (pin == DEEPSLEEP_PIN_PD6) ? EXTI_Line6 : 
                        (pin == DEEPSLEEP_PIN_PA2) ? EXTI_Line2 :
                        (pin == DEEPSLEEP_PIN_PC2) ? EXTI_Line2 :
                        (pin == DEEPSLEEP_PIN_PC1) ? EXTI_Line1 : EXTI_Line4;
    
    // Enable interrupt mask
    EXTI->INTENR |= exti_line;
    
    // Configure trigger type
    switch (trigger) {
        case DEEPSLEEP_TRIGGER_FALLING:
            EXTI->FTENR |= exti_line;
            EXTI->RTENR &= ~exti_line;
            break;
        case DEEPSLEEP_TRIGGER_RISING:
            EXTI->RTENR |= exti_line;
            EXTI->FTENR &= ~exti_line;
            break;
        case DEEPSLEEP_TRIGGER_BOTH:
            EXTI->FTENR |= exti_line;
            EXTI->RTENR |= exti_line;
            break;
    }
    
    // Enable EXTI interrupt in NVIC
    NVIC_EnableIRQ(EXTI7_0_IRQn);
}

// Enter deep sleep mode
void deepsleep_enter(void) {
    wake_up_flag = 0;
    
    if (deepsleep_cfg.wake_method == DEEPSLEEP_WAKE_TIMER) {
        // Start the timer
        TIM1->CNT = 0;  // Reset counter
        TIM1->CTLR1 |= TIM_CEN;  // Enable timer
    }
    
    // Clear any pending wake-up flags
    PWR->CSR |= PWR_CSR_WUF;
    
    // Enable PWR clock
    RCC->APB1PCENR |= RCC_APB1Periph_PWR;
    
    // Configure sleep mode (Sleep mode, not Stop or Standby)
    PWR->CTLR &= ~PWR_CTLR_PDDS;  // Sleep mode
    
    // Enter sleep mode and wait for interrupt
    while (!wake_up_flag) {
        __WFI();  // Wait For Interrupt
    }
}

// Check if MCU is awake
uint8_t deepsleep_is_awake(void) {
    return wake_up_flag;
}

// Reset wake-up flag
void deepsleep_reset_wake_flag(void) {
    wake_up_flag = 0;
}