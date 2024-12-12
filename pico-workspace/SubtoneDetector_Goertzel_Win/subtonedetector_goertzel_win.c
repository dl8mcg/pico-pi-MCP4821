/* Raspberry Pico Pi subtone goertzel detector with gauss window function demo */

/*
*   ADCin --> DFT 123 Hz subtone detect 
*
*   by dl8mcg Nov. 2024
*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/irq.h"
#include "hardware/adc.h"

#include "goertzelwin.h"

/*
    ADC0 (pin31)    <-  AFin
*/

#define LED_OB 25                                           // GPIO25 green onboard LED
#define SUB_DET_LED 6                                       // GPIO6 (pin 9) subtone detect LED indication
#define IRQ_LOAD 16                                         // GPIO16 (pin 21) interrupt load time

const float subtone_frequency = 123.0;                      // subtone-frequency to be detected

const int number_of_samples = 3000;                         // number of samples to be processed

const float sample_frequency = 13333.3333;                  // ADC sample rate in Hz

/*----- ADC ------*/
volatile float adcval;
volatile float adcval_signed;                               // signed and normalized ADC-Value

/*----- goertzel ------*/
static float amplitude;                                     // processed amplitude of subtone
volatile uint16_t cnt = 0;                                  // counter for LED toggle

void timer_irq_handler() 
{
    gpio_put(IRQ_LOAD,1);                                   // enter IRQ-sequence

    adc_run(true);                                          // start next AD - conversion
    
    timer_hw->intr = 1u << 0;                               // Timer-Interrupt-Flag reset
    
    timer_hw->alarm[0] = timer_hw->timerawl + 1e6 / sample_frequency;    // set next timer interrupt (75 µs)

    adcval = (float) adc_fifo_get();                        // fetch ADC-value from fifo

    adcval_signed = (adcval - 2047.0) / 2048.0;             // shift to +/- and normalize 12 bit ADC - Signal

    process_goertzel_sample(adcval_signed);                 // process subtone DFT detection

    cnt++;
    if(cnt>1000)
    {
        gpio_xor_mask(1u << LED_OB);                        // toggle on board LED alife indicator
        cnt = 0;
    }

    gpio_put(IRQ_LOAD,0);                                   // leave IRQ-sequence
}

int main ()
{
    // GPIO config for on board LED and subtone detector LED
    gpio_init(LED_OB);
    gpio_set_dir(LED_OB, GPIO_OUT);
    gpio_put(LED_OB,0);
    gpio_init(SUB_DET_LED);
    gpio_set_dir(SUB_DET_LED, GPIO_OUT);
    gpio_init(IRQ_LOAD);
    gpio_set_dir(IRQ_LOAD,GPIO_OUT);

    // Timer-IRQ sample rate
    timer_hw->alarm[0] = timer_hw->timerawl + 1e6 / sample_frequency;    // set next timer interrupt (75 µs)
    
    irq_set_exclusive_handler(TIMER_IRQ_0, timer_irq_handler);
    irq_set_enabled(TIMER_IRQ_0, true);
    
    adc_init();                                                     // ADC reset  
    adc_gpio_init(26);                                              // ADC channel 0 input
    adc_select_input(0);                                            // ADC channel 0 select

    adc_fifo_setup(true,false,1,false,false);                       // ADC get from FIFO

    initialize_goertzel(subtone_frequency,sample_frequency,number_of_samples);     // init 123 Hz subtone detector
    
    // Timer-Interrupt enable
    timer_hw->inte = 1u << 0;

    stdio_init_all();
 
    while(1)                                                        // main loop 
    {
        amplitude = get_goertzel_amplitude();                       // get amplitude of subtone signal        

        if(amplitude > 0.150)                                       // threshold in V
            gpio_put(SUB_DET_LED,1);                                // SUB_DET_LED-indication on
        else
            gpio_put(SUB_DET_LED,0);                                // SUB_DET_LED-indication off

        printf("amplitude : %.3f V\n",amplitude);

        sleep_ms(222);
    }
}


