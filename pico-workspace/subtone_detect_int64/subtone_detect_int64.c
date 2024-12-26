/*
*   by dl8mcg Dec. 2024
*/

/* Raspberry Pico Pi Subtone Detect Int64 demo*/

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "sampleprocessing.h"
#include "detect_subtone.h"

static float ampli_123Hz;                                               // processed ampli_123Hz of subtone

int main ()
{
    set_sys_clock_khz(133000, true);                                    // sets clock frequency to 133 MHz and waits for proper initialization
    stdio_init_all();                                                   // initialize standard I/O functions

    gpio_init(LED_OB);
    gpio_set_dir(LED_OB, GPIO_OUT);
    initialize_subdet();                                                // init 123 Hz subtone detector   
    initialize_timer_and_adc();

    while(1)                                                            // loop 
    {
        ampli_123Hz = get_subdet_amplitude();                           // get amplitude of 123 Hz subtone signal  

        if(ampli_123Hz > 0.150)                                         // threshold subtone in V
            gpio_put(SUB_DET_LED,1);                                    // SUB_DET_LED-indication on
        else
            gpio_put(SUB_DET_LED,0);                                    // SUB_DET_LED-indication off

        printf("det_123Hz : %.3f V     \n", ampli_123Hz);

        gpio_xor_mask(1u << LED_OB);                                    // Onboard-LED toggeln
        
        sleep_ms(222);
    }
}


