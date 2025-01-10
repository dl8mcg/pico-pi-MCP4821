/*
*   by dl8mcg Jan. 2025
*/

/* Raspberry Pico Pi    2FSK-demodulator with RTTY-, ASCII and AX25-decoder    floating point arithmetic calculation    demo */

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "sampleprocessing.h"
#include "fsk_demod.h"

#include "fsk_decode_rtty.h"
#include "fsk_decode_ascii.h"
#include "fsk_decode_ax25.h"
#include "dac.h"

uint tgcnt = 0;                                                         // LED toggle counter

int main ()
{
    set_sys_clock_khz(133000, true);                                    // sets clock frequency to 133 MHz and waits for proper initialization
    stdio_init_all();                                                   // initialize standard I/O functions

    init_OB_LED;                                                        // init on board LED

    initialize_spi_dac();                                               // initialize SPI for DAC
    
    initialize_fsk();                                                   // initialize fsk detector
    
    initialize_timer_and_adc();                                         // initialize signal processing timer and ADC

    while(1)                                                            // loop main program
    {

        if(demready)
        {
            demready = false;
            //process_rtty(demod_bit);
            //process_ascii(demod_bit);
            process_ax25(demod_bit); 
        }

        if(decready)
        {
            decready = false;
            printf("%c",decoded_char);                                  // send demodulated and decoded characters to uart
        }

        tgcnt++;
        if(tgcnt > 2000000)
        {
            toggle_OB_LED;                                              // toggle on board LED as live indicator
            tgcnt = 0;
        }
    }
}


