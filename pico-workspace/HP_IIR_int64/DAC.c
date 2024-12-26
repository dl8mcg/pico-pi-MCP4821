/*
*   by dl8mcg Dec. 2024
*/

/* 
   DAC MCP4821 must be supplied with 3.3 V, not 5 V, violation of minimum high level requirement
   Pico Pi          ->   MCP4821
   GPIO17 (pin 22)  ->   /CS (pin 2) 
   GPIO18 (pin 24)  ->   SCK (pin 3) 
   GPIO19 (pin 25)  ->   SDI (pin 4)
   3.3V (pin 36)    ->   VCC (pin 1, pin 6)
   GND (pin 38)     ->   GND (pin 7, pin 5)
*/

#include "config.h"
#include "DAC.h"
#include "hardware/spi.h"
#include "hardware/irq.h"
#include "pico/stdlib.h"

static volatile uint16_t spi_cnt = 0;

void spi_write(uint16_t data) 
{
    // spi_get_hw(spi_default)->imsc |= (1 << 3);  // Activate txempty IRQ  // <-- don´t delete
    spi_get_hw(spi_default)->dr = data;         // Write data to SPI Tx register
}

void spi_irq_handler() 
{
    spi_get_hw(spi_default)->imsc &= ~(1 << 3);  // Deactivate txempty IRQ
    spi_cnt++;
    if (spi_cnt > 1000)
    {
        gpio_xor_mask(1u << LED_OB);  // Toggle on-board LED indicator
        spi_cnt = 0;
    }
}

void initialize_spi()
{
    // SPI interface configuration
    spi_init(spi_default, 20000000);                               // Baud rate = 20 Mbit/s
    gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);    // SPI-SCK GPIO18
    gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);     // SPI-TX GPIO19
    gpio_set_function(PICO_DEFAULT_SPI_CSN_PIN, GPIO_FUNC_SPI);    // CS-pin GPIO17

    spi_set_format(spi_default, 16, 0, 0, SPI_MSB_FIRST);          // SPI format (slope, direction, bitcount)

    // SPI interrupt configuration                             // <-- don´t delete
    // irq_set_exclusive_handler(SPI0_IRQ, spi_irq_handler);   // <-- don´t delete
    // irq_set_enabled(SPI0_IRQ, true);                        // <-- don´t delete

    // GPIOs
    gpio_init(LED_OB);
    gpio_set_dir(LED_OB, GPIO_OUT);
    gpio_put(LED_OB,0);
}
