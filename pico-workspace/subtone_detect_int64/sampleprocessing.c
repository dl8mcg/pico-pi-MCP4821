/*
*   by dl8mcg Dec. 2024
*/

/*
    ADC0 (pin31)    <-  AFin
*/

#include "config.h"
#include "sampleprocessing.h"
#include "hardware/adc.h"
#include "hardware/timer.h"
#include "hardware/gpio.h"
#include "detect_subtone.h"

volatile static uint32_t alarm_time;  
volatile static int16_t adcval;

void timer_irq_handler()
{
    gpio_put(IRQ_LOAD, 1);                                  // IRQ-start

    adc_run(true);                                          // start AD-conversion
    timer_hw->intr = 1u << 0;                               // clear timer-interrupt-flag
    timer_hw->alarm[0] = timer_hw->timerawl + alarm_time;   // set next timer-interrupt

    adcval = adc_fifo_get();                                // fetch 12-bit-ADC-value from fifo
    adcval -= 0x7FF;                                        // signed integer
    process_subdet_sample(adcval);                          // process subtone detection

    gpio_put(IRQ_LOAD, 0);                                  // IRQ-end
}

void initialize_timer_and_adc()
{
    alarm_time = (uint32_t)(1e6 / fsample);                 // interval in microseconds
    timer_hw->alarm[0] = timer_hw->timerawl + alarm_time;

    // Timer-Interrupt config
    irq_set_exclusive_handler(TIMER_IRQ_0, timer_irq_handler);
    irq_set_priority(TIMER_IRQ_0, 0);                       // set highest priority (0 = highest)
    irq_set_enabled(TIMER_IRQ_0, true);

    // ADC config
    adc_init();
    adc_gpio_init(26);                                      // ADC-chanel 0 (GPIO26)
    adc_select_input(0);
    adc_fifo_setup(true, false, 1, false, false);

    gpio_init(IRQ_LOAD);
    gpio_set_dir(IRQ_LOAD, GPIO_OUT);

    // Timer-Interrupt enable
    timer_hw->inte = 1u << 0;
}
