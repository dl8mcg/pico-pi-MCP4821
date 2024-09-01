/* Raspberry Pico Pi to set DAC MCP4821 via SPI - non blocking - timer IRQ - sine waveform demo*/

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/irq.h"
#include <math.h>

/* 
   DAC MCP4821 must be supplied with 3.3 V, not 5 V, violation of minimum high level requirement
   Pico Pi          -> MCP4821
   GPIO17 (pin 22)  -> /CS (pin 2) 
   GPIO18 (pin 24)  -> SCK (pin 3) 
   GPIO19 (pin 25)  -> SDI (pin 4)
   3.3V (pin 36)    -> VCC (pin 1, pin 6)
   GND (pin 38)     -> GND (pin 7, pin 5)
*/

#define LED_OB 25                               // GPIO25 for green onboard LED

volatile uint32_t alarm_time;

const float freq = 123.0;                       // output sine frequency

volatile uint16_t cnt = 0;
volatile int16_t dacval = 0;
volatile float rad = 0.0;

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
    // Timer-Interrupt-Flag reset
    timer_hw->intr = 1u << 0;

     // set next timer interrupt
    timer_hw->alarm[0] = timer_hw->timerawl + alarm_time;

    // calculate sine - value
    dacval = 0x0FFF / 2 + 0x0FFF / 2 * sin(rad);         // calculate next sine - DAC - value

    rad += freq / 13333.3333 * 2 * M_PI;                 // add partial angle

    if(rad > 2 * M_PI)                                   // keep within 0 to 2pi rad
        rad -= 2 * M_PI;

    dacval &= 0x0FFF;                                    // limit to DAC full scale
    spi_write(dacval + 0x3000);                          // add control bits and send to MCP4821 (see datasheet)
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
    alarm_time = 75;                                       // DAC sampling rate = 13.333 kHz is 75 us
    timer_hw->alarm[0] = timer_hw->timerawl + alarm_time;
    
    // Timer-IRQ config
    irq_set_exclusive_handler(TIMER_IRQ_0, timer_irq_handler);
    irq_set_enabled(TIMER_IRQ_0, true);
    
    // Timer-Interrupt enable
    timer_hw->inte = 1u << 0;
 
    while(1)                                            // loop 
    {
       // nothing
    }
}


