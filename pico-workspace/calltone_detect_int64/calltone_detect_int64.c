/*
*   by dl8mcg Dec. 2024
*/

/* Raspberry Pico Pi Calltone Detect Int64 demo*/

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "sampleprocessing.h"
#include "detect_calltone.h"

static float ampli_1750Hz;                                             // processed ampli_1750Hz of calltone

int main ()
{
    set_sys_clock_khz(133000, true);                                    // sets clock frequency to 133 MHz and waits for proper initialization
    stdio_init_all();                                                   // initialize standard I/O functions

    gpio_init(LED_OB);
    gpio_set_dir(LED_OB, GPIO_OUT);
    initialize_calldet();                                              // init 1750 Hz calltone detector   
    initialize_timer_and_adc();
    stdio_init_all();

    while(1)                                                           // loop 
    {
        ampli_1750Hz = get_calldet_amplitude();                        // get amplitude of 1750 Hz calltone signal  

        if(ampli_1750Hz > 0.700)                                       // threshold calltone in V
            gpio_put(CT_DET_LED,1);                                    // CT_DET_LED-indication on
        else
            gpio_put(CT_DET_LED,0);                                    // CT_DET_LED-indication off

        printf("det_1750Hz : %.3f V     \n", ampli_1750Hz);

        gpio_xor_mask(1u << LED_OB);                                    // toggle Onboard-LED 
        
        sleep_ms(222);
    }
}


