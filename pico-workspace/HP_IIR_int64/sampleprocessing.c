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
#include "hpiir.h"
#include "DAC.h"

static uint32_t alarm_time;  
static int16_t adcval;

static int16_t dacval;

void timer_irq_handler()
{
    gpio_put(IRQ_LOAD, 1);                                  // IRQ-Startsignal 

    adc_run(true);                                          // Start der AD-Konvertierung
    timer_hw->intr = 1u << 0;                               // Timer-Interrupt-Flag zurücksetzen
    timer_hw->alarm[0] = timer_hw->timerawl + alarm_time;   // Nächsten Timer-Interrupt setzen

    adcval = adc_fifo_get();                                // 12-bit-ADC-Wert abrufen
    adcval -= 0x7FF;

    dacval = (int16_t) filter(adcval);

    dacval += 0x7FF;
    dacval &= 0x0FFF;
    dacval += 0x3000;
    spi_write(dacval);                                      // 12-bit-DAC

    gpio_put(IRQ_LOAD, 0);                                  // IRQ-Endsignal 
}

void initialize_timer_and_adc()
{
    alarm_time = (uint32_t)(1e6 / fsample);                 // Intervallzeit in Mikrosekunden berechnen
    timer_hw->alarm[0] = timer_hw->timerawl + alarm_time;

    // Timer-Interrupt konfigurieren
    irq_set_exclusive_handler(TIMER_IRQ_0, timer_irq_handler);
    irq_set_priority(TIMER_IRQ_0, 0);                       // Höchste Priorität setzen (0 = höchste Priorität)
    irq_set_enabled(TIMER_IRQ_0, true);

    // ADC konfigurieren
    adc_init();
    adc_gpio_init(26);                                      // ADC-Kanal 0 (GPIO26)
    adc_select_input(0);
    adc_fifo_setup(true, false, 1, false, false);

    gpio_init(IRQ_LOAD);
    gpio_set_dir(IRQ_LOAD, GPIO_OUT);

    // Timer-Interrupt enable
    timer_hw->inte = 1u << 0;
}
