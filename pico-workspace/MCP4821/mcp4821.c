/* Raspberry Pico Pi to set DAC MCP4821 via SPI - demo*/

#include "pico/stdlib.h"
#include "hardware/spi.h"

/* 
   DAC MCP4821 must be supplied with 3.3 V, not 5 V, violation of minimum high level requirement
   Pico Pi          -> MCP4821
   GPIO17 (pin 22)  -> /CS (pin 2) 
   GPIO18 (pin 24)  -> SCK (pin 3) 
   GPIO19 (pin 25)  -> SDI (pin 4)
   3.3V (pin 36)    -> VCC (pin 1, pin 6)
   GND (pin 38)     -> GND (pin 7, pin 5)
*/

#define CS_DAC 17           // GPIO17 pin 22  /CS DAC MCP4821
#define LED_OB 25           // GPIO25 for green onboard LED

uint16_t spiBuf16[1];

int main ()
{
    // GPIO init for onboard LED
    gpio_init(LED_OB);
    gpio_set_dir(LED_OB, GPIO_OUT);
    gpio_put(LED_OB,1);

    // SPI-Interface init
    spi_init(spi_default,  10000000);                               // baud rate = 10 MBit/s
    gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);     // configure SPI-SCK-pin
    gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);      // configure SPI-TX-pin
    spi_set_format(spi_default,16,0,0,SPI_MSB_FIRST) ;              // configure SPI-format (slope, direction, bitcount) 

    // configure Chip-Select for DAC 
    gpio_init(CS_DAC);
    gpio_set_dir(CS_DAC, GPIO_OUT);
    gpio_put(CS_DAC,1);

    while(1)                                // loop to create a stair step waveform
    {
        spiBuf16[0] = 0x3000;               // MCP4821 control + 0x000  -> 0 V
        gpio_put(CS_DAC,0);
        spi_write16_blocking(spi_default, spiBuf16, 1);
        gpio_put(CS_DAC,1);
        gpio_xor_mask(1u << LED_OB);        // toggle on board LED 
        sleep_us(100);

        spiBuf16[0] = 0x33FF;               // MCP4821 control + 0x3FF  -> 511 mV
        gpio_put(CS_DAC,0);
        spi_write16_blocking(spi_default, spiBuf16, 1);
        gpio_put(CS_DAC,1);
        gpio_xor_mask(1u << LED_OB);        // toggle on board LED 
        sleep_us(100);        

        spiBuf16[0] = 0x37FF;               // MCP4821 control + 0x7FF  -> 1024 mV
        gpio_put(CS_DAC,0);
        spi_write16_blocking(spi_default, spiBuf16, 1);
        gpio_put(CS_DAC,1);
        gpio_xor_mask(1u << LED_OB);        // toggle on board LED 
        sleep_us(100);

        spiBuf16[0] = 0x3BFF;               // MCP4821 control + 0xBFF  -> 1535 mV
        gpio_put(CS_DAC,0);
        spi_write16_blocking(spi_default, spiBuf16, 1);
        gpio_put(CS_DAC,1);
        gpio_xor_mask(1u << LED_OB);        // toggle on board LED 
        sleep_us(100);

        spiBuf16[0] = 0x3FFF;               // MCP4821 control + 0xFFF  -> 2047 mV
        gpio_put(CS_DAC,0);
        spi_write16_blocking(spi_default, spiBuf16, 1);
        gpio_put(CS_DAC,1);
        gpio_xor_mask(1u << LED_OB);        // toggle on board LED 
        sleep_us(100);     
    }
}


