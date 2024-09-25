/* Raspberry Pico Pi DAC MCP4821 ADC to DAC demo*/

/*
* send ADC data to DAC
*/

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/irq.h"
#include <math.h>
#include "hardware/adc.h"

/* 
   DAC MCP4821 must be supplied with 3.3 V, not 5 V, violation of minimum high level requirement
   Pico Pi          ->   MCP4821
   GPIO17 (pin 22)  ->   /CS (pin 2) 
   GPIO18 (pin 24)  ->   SCK (pin 3) 
   GPIO19 (pin 25)  ->   SDI (pin 4)
   3.3V (pin 36)    ->   VCC (pin 1, pin 6)
   GND (pin 38)     ->   GND (pin 7, pin 5)
*/

/*
    ADC0 (pin31)    <-  AFin
*/

#define LED_OB 25                               // GPIO25 for green onboard LED

volatile float fsample = 13333.3333;            // DAC sample rate in Hz
volatile uint32_t alarm_time;  

volatile uint16_t cnt = 0;
volatile uint16_t dacval = 0;
volatile uint16_t adcval;

void spi_write(uint16_t data);
void timer_irq_handler();


void spi_write(uint16_t data) 
{
    spi_get_hw(spi_default)->imsc |= (1 << 3);  // activate txempty-irq
    spi_get_hw(spi_default)->dr = data;         // write data to SPI-Tx-register
}

void spi_irq_handler() 
{
    spi_get_hw(spi_default)->imsc &= ~(1 << 3);  // deactivate txempty-irq, Lösche das dritte Bit im IMSC-Register
    cnt++;
    if(cnt>1000)
    {
        gpio_xor_mask(1u << LED_OB);            // toggle on board LED indicator
        cnt = 0;
    }
}

void timer_irq_handler() 
{
    adc_run(true);                  // start conversion
    
    timer_hw->intr = 1u << 0;       // Timer-Interrupt-Flag reset
    
    timer_hw->alarm[0] = timer_hw->timerawl + alarm_time;   // set next timer interrupt

    adcval = adc_fifo_get();        // fetch from fifo

    dacval = adcval;

    dacval &= 0x0FFF;               // limit to DAC-fullscale
    spi_write(dacval + 0x3000);     // add control-bits and send to DAC 
}


int main ()
{
    // GPIO config for on board LED
    gpio_init(LED_OB);
    gpio_set_dir(LED_OB, GPIO_OUT);
    gpio_put(LED_OB,0);

    // SPI-Interface config
    spi_init(spi_default,  20000000);                               // baud rate = 20 MBit/s
    gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);     // SPI-SCK GPIO18 pin 24
    gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);      // SPI-TX GPIO19 pin 25
    gpio_set_function(PICO_DEFAULT_SPI_CSN_PIN, GPIO_FUNC_SPI);     // CS-pin GPIO17 pin 22  /CS DAC MCP4821
    spi_set_format(spi_default,16,0,0,SPI_MSB_FIRST) ;              // SPI-format (slope, direction, bitcount) 

    // SPI-Interrupt config    <-- just for fun to see OB LED SPI Tx underrun indication (not necessary)
    irq_set_exclusive_handler(SPI0_IRQ, spi_irq_handler);
    irq_set_enabled(SPI0_IRQ, true);
    
    // Timer-IRQ config time
    alarm_time = 1e6 / fsample;                            // 75 us  
    timer_hw->alarm[0] = timer_hw->timerawl + alarm_time;
    
    // Timer-IRQ config
    irq_set_exclusive_handler(TIMER_IRQ_0, timer_irq_handler);
    irq_set_enabled(TIMER_IRQ_0, true);
    
    adc_init();
    adc_gpio_init(26);  //channel 0
    adc_select_input(0);

    adc_fifo_setup(true,false,1,false,false);

    // Timer-Interrupt enable
    timer_hw->inte = 1u << 0;
 
    while(1)                                            // loop 
    {
       // nothing
    }
}


