/* Raspberry Pico Pi to set DAC MCP4821 via SPI - non blocking IRQ - demo*/

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/irq.h"

/* 
   DAC MCP4821 must be supplied with 3.3 V, not 5 V, violation of minimum high level requirement
   Pico Pi          -> MCP4821
   GPIO17 (pin 22)  -> /CS (pin 2) 
   GPIO18 (pin 24)  -> SCK (pin 3) 
   GPIO19 (pin 25)  -> SDI (pin 4)
   3.3V (pin 36)    -> VCC (pin 1, pin 6)
   GND (pin 38)     -> GND (pin 7, pin 5)
*/

#define LED_OB 25           // GPIO25 for green onboard LED

uint16_t cnt = 0;

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
        gpio_xor_mask(1u << LED_OB);                    // toggle on board LED indicator
        cnt = 0;
    }
}

int main ()
{
    // GPIO init for onboard LED
    gpio_init(LED_OB);
    gpio_set_dir(LED_OB, GPIO_OUT);
    gpio_put(LED_OB,0);

    // SPI-Interface init
    spi_init(spi_default,  20000000);                               // baud rate = 10 MBit/s
    gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);     // configure SPI-SCK-pin
    gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);      // configure SPI-TX-pin
    gpio_set_function(PICO_DEFAULT_SPI_CSN_PIN, GPIO_FUNC_SPI);     // CS-pin GPIO17 pin 22  /CS DAC MCP4821
    spi_set_format(spi_default,16,0,0,SPI_MSB_FIRST) ;              // configure SPI-format (slope, direction, bitcount) 

    // SPI-Interrupt aktivieren
    irq_set_exclusive_handler(SPI0_IRQ, spi_irq_handler);
    irq_set_enabled(SPI0_IRQ, true);
 
    while(1)                                // loop to create a stair step waveform
    {
        spi_write(0x3000);
        sleep_us(50);

        spi_write(0x33FF);
        sleep_us(50);

        spi_write(0x37FF);
        sleep_us(50);

        spi_write(0x3BFF);
        sleep_us(50);

        spi_write(0x3FFF);
        sleep_us(50);
    }
}


