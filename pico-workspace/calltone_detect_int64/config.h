/*
*   by dl8mcg Dec. 2024
*/

#pragma once
#ifndef CONFIG_H
#define CONFIG_H

#define LED_OB 25                       // GPIO25 for green onboard LED
#define CT_DET_LED 6                    // GPIO6 (pin 9) call tone detect LED indication
#define IRQ_LOAD 16                     // GPIO16 (pin 21) interrupt load time 

#define fsample  13333.3333             // DAC sample rate in Hz
#define calltone_frequency 1750.0       // calltone-frequency to be detected
#define calltone_samples 1000           // number of samples to be processed
#define calltone_sigma 0.2              // standard deviation (typically: 0.2 bis 0.5); here 0.2 for lower frequency resolution 


#endif // CONFIG_H

