/* Raspberry Pico Pi DAC MCP4821 IIR Filter Oszillator mit Fixed-Point-Arithmetik (Q24) */

/*
* IIR - Filter - Oszillator
* Y(n) = SIN_OMEGA * dirac(n) + 2*COS_OMEGA*Y(n-1) - Y(n-2)
*/

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/irq.h"
#include <math.h>

// Konstanten
#define LED_OB 25                    // GPIO25 für grüne Onboard-LED
#define Q24_SHIFT 24                 // Anzahl der Bits für Q24-Festkommaformat
#define DAC_FULLSCALE 0xFFF          // 12-Bit DAC-Auflösung

// Parameter
const float ftone = 750.0;            // Ausgabe-Sinusfrequenz in Hz
const float fsample = 13333.3333;    // DAC-Abtastrate in Hz
const uint32_t alarm_time = 1e6 / fsample;

// Globale Variablen
volatile uint16_t cnt = 0;
volatile int64_t two_coswt_q24, sinwt_q24;
volatile int64_t yn0_q24 = 0, yn1_q24 = 0, yn2_q24 = 0;

// Funktionsprototypen
void init_osc();
void spi_write(uint16_t data);
void timer_irq_handler();

void init_osc() 
{
    // Amplitude einstellen (z. B. 99,9% des DAC-Vollaussteuerungsbereichs)
    float amplitude_factor = 0.999; 
    int64_t amplitude_q24 = (int64_t)((DAC_FULLSCALE / 2.0) * amplitude_factor * (1 << Q24_SHIFT));

    // Koeffizienten im Q24-Format berechnen
    float omega = 2.0 * M_PI * ftone / fsample;
    float coswt = cos(omega);
    float sinwt = sin(omega);

    two_coswt_q24 = (int64_t)(2.0 * coswt * (1 << Q24_SHIFT));
    sinwt_q24 = (int64_t)(sinwt * (1 << Q24_SHIFT));

    // Dirac-Impuls initialisieren
    int64_t dirac_q24 = (amplitude_q24 * sinwt_q24) >> Q24_SHIFT;

    // Initialisierung der IIR-Zustände
    yn2_q24 = 0;              // y(n-2) = 0
    yn1_q24 = dirac_q24;      // y(n-1) = Dirac-Impuls
}

void spi_write(uint16_t data) 
{
    spi_get_hw(spi_default)->imsc |= (1 << 3);  // Aktivieren des TX-Empty-IRQs
    spi_get_hw(spi_default)->dr = data;         // Daten ins SPI-TX-Register schreiben
}

void spi_irq_handler() 
{
    spi_get_hw(spi_default)->imsc &= ~(1 << 3); // Deaktivieren des TX-Empty-IRQs
    cnt++;
    if (cnt > 1000) {
        gpio_xor_mask(1u << LED_OB);            // Onboard-LED toggeln
        cnt = 0;
    }
}

void timer_irq_handler() 
{
    // Timer-Interrupt-Flag zurücksetzen
    timer_hw->intr = 1u << 0;

    // Nächsten Timer-Interrupt setzen
    timer_hw->alarm[0] = timer_hw->timerawl + alarm_time;

    // IIR-Filter berechnen
    yn0_q24 = (two_coswt_q24 * yn1_q24 >> Q24_SHIFT) - yn2_q24;

    // Zustände aktualisieren
    yn2_q24 = yn1_q24;
    yn1_q24 = yn0_q24;

    // DAC-Wert berechnen
    int64_t dacval = yn0_q24 >> Q24_SHIFT;    // Q24 zurück in Integer konvertieren
    dacval += DAC_FULLSCALE / 2;              // DC-Offset hinzufügen
    dacval &= DAC_FULLSCALE;                  // Auf 12-Bit DAC-Auflösung begrenzen

    spi_write(dacval | 0x3000);               // Wert an DAC senden
}

int main() 
{
    // GPIO für Onboard-LED konfigurieren
    gpio_init(LED_OB);
    gpio_set_dir(LED_OB, GPIO_OUT);
    gpio_put(LED_OB, 0);

    // SPI-Schnittstelle konfigurieren
    spi_init(spi_default, 20000000);                              // Baudrate: 20 MBit/s
    gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);   // SPI-SCK GPIO18
    gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);    // SPI-TX GPIO19
    gpio_set_function(PICO_DEFAULT_SPI_CSN_PIN, GPIO_FUNC_SPI);   // CS GPIO17
    spi_set_format(spi_default, 16, 0, 0, SPI_MSB_FIRST);         // SPI-Format einstellen

    // SPI-Interrupt konfigurieren (nur zur Anzeige auf der LED)
    irq_set_exclusive_handler(SPI0_IRQ, spi_irq_handler);
    irq_set_enabled(SPI0_IRQ, true);

    // Timer-Interrupt konfigurieren
    timer_hw->alarm[0] = timer_hw->timerawl + alarm_time;
    irq_set_exclusive_handler(TIMER_IRQ_0, timer_irq_handler);
    irq_set_enabled(TIMER_IRQ_0, true);

    // Oszillator initialisieren
    init_osc();

    // Timer-Interrupt aktivieren
    timer_hw->inte = 1u << 0;

    // Endlosschleife
    while (1) 
    {
        // Leerlaufschleife
    }
}
