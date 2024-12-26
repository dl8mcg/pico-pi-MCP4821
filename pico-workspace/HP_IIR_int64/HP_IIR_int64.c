/*
*   by dl8mcg Dec. 2024
*/

/* Raspberry Pico Pi IIR highpass filter int64 fixed point arithmetic demo*/

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "sampleprocessing.h"
#include "hpiir.h"
#include "DAC.h"

static float x;

int main ()
{
    set_sys_clock_khz(133000, true);                                    // sets clock frequency to 133 MHz and waits for proper initialization
    stdio_init_all();                                                   // initialize standard I/O functions

    gpio_init(LED_OB);
    gpio_set_dir(LED_OB, GPIO_OUT);

    initialize_spi();
    initialize_timer_and_adc();
    stdio_init_all();

    while(1)                                                           // loop 
    {
        printf("int64 fp hpiir : %.9f \n", x);
        x += 0.123456789;

        gpio_xor_mask(1u << LED_OB);                                   // Onboard-LED 
        
        sleep_ms(222);
    }
}


