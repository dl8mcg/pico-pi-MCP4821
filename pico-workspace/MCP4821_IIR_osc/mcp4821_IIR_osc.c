/* Raspberry Pico Pi DAC MCP4821 IIR filter oscillator demo*/

/*
* IIR - Filter - Oscillator
* Y(n) = SIN_OMEGA * dirac(n) + 2*COS_OMEGA*Y(n-1) - Y(n-2)
*/

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/irq.h"
#include <math.h>

/* 
   DAC MCP4821 must be supplied with 3.3 V, not 5 V, violation of minimum high level requirement
   Pico Pi          ->   MCP4821
   GPIO17 (pin 22)  ->   /CS (pin 2) 
   GPIO18 (pin 24)  ->   SCK (pin 3) 
   GPIO19 (pin 25)  ->   SDI (pin 4)
   3.3V (pin 36)    ->   VCC (pin 1, pin 6)
   GND (pin 38)     ->   GND (pin 7, pin 5)
*/

#define LED_OB 25                               // GPIO25 for green onboard LED

const float ftone = 1234.0;                     // output sine frequency in Hz
const float fsample = 13333.3333;               // DAC sample rate in Hz
const uint32_t alarm_time =  1e6 / fsample;   

volatile uint16_t cnt = 0;
volatile int16_t dacval = 0;

volatile float coswt;
volatile float two_coswt;
volatile float sinwt;
volatile float amplitude;
volatile float dirac;
volatile float yn1 = 0.0;
volatile float yn2 = 0.0;
volatile float yn0 = 0.0;

void init_osc();
void spi_write(uint16_t data);
void timer_irq_handler();

void init_osc()
{
	amplitude = 0xFFF/2;                        // set amplitude to DAC-fullscale half
	coswt = cos(2.0 * M_PI * ftone / fsample);  
	two_coswt = 2.0 * coswt;
	sinwt = sin(2.0 * M_PI * ftone / fsample);
	dirac = amplitude * sinwt ;
	
	yn0 = dirac + two_coswt * yn1  - yn2;
	yn2 = yn1;
	yn1 = yn0;
}

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

    // calculate oscillating IIR - filter
	yn0 = two_coswt * yn1  - yn2;
	yn2 = yn1;
	yn1 = yn0;

    dacval = (int16_t)yn0;              // float to integer
    dacval += 0xFFF/2;                  // add DC offset (DAC-fullscale half)
    dacval &= 0x0FFF;                   // limit to DAC-fullscale

    spi_write(dacval + 0x3000);         // add control-bits and send to DAC 
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
    timer_hw->alarm[0] = timer_hw->timerawl + alarm_time;
    
    // Timer-IRQ config
    irq_set_exclusive_handler(TIMER_IRQ_0, timer_irq_handler);
    irq_set_enabled(TIMER_IRQ_0, true);
    
    init_osc();

    // Timer-Interrupt enable
    timer_hw->inte = 1u << 0;
 
    while(1)                            // loop 
    {
       // nothing
    }
}


