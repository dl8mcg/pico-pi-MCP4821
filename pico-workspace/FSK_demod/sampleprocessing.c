/*
*   by dl8mcg Jan. 2025
*/

/*
    ADC0 (pin31)    <-  AFin
*/

#include "config.h"
#include "sampleprocessing.h"
#include "hardware/adc.h"
#include "hardware/timer.h"
#include "hardware/gpio.h"
#include "fsk_demod.h"
#include "dac.h"

volatile static uint32_t alarm_time;  
volatile static int16_t adcval;

void timer_irq_handler()                                    // Timer-Interrupt : get ADC sample , demodulate FSK
{
    gpio_put(IRQ_LOAD, 1);                                  // IRQ-start 

    adc_run(true);                                          // Start der AD-Konvertierung
    timer_hw->intr = 1u << 0;                               // reset timer interrupt flag
    timer_hw->alarm[0] = timer_hw->timerawl + alarm_time;   // set next timer interrupt time

    adcval = adc_fifo_get();                                // fetch 12-bit-ADC-value from FIFO

    process_fsk_demodulation(adcval);                       // downconvert and demodulate FSK - signal

    write_dac(adcval);                                      // just for audio monitoring via DAC
    
    gpio_put(IRQ_LOAD, 0);                                  // IRQ-end 
}

void initialize_timer_and_adc()
{
    alarm_time = (uint32_t)(1e6 / SAMPLING_RATE);           // calculate interval time in microseconds
    timer_hw->alarm[0] = timer_hw->timerawl + alarm_time;

    // Timer-Interrupt konfigurieren
    irq_set_exclusive_handler(TIMER_IRQ_0, timer_irq_handler);
    irq_set_priority(TIMER_IRQ_0, 0);                       // set highes interrupt priority (0 = highest priority)
    irq_set_enabled(TIMER_IRQ_0, true);

    // ADC konfigurieren
    adc_init();
    adc_gpio_init(26);                                      // ADC 0 (GPIO26) is input
    adc_select_input(0);
    adc_fifo_setup(true, false, 1, false, false);

    gpio_init(IRQ_LOAD);
    gpio_set_dir(IRQ_LOAD, GPIO_OUT);

    // Timer-Interrupt enable
    timer_hw->inte = 1u << 0;
}
