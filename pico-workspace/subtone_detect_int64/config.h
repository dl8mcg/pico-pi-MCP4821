/*
*   by dl8mcg Dec. 2024
*/

#pragma once
#ifndef CONFIG_H
#define CONFIG_H

#define LED_OB 25                       // GPIO25 for green onboard LED
#define SUB_DET_LED 7                   // GPIO7 (pin 10) subtone detect LED indication
#define IRQ_LOAD 16                     // GPIO16 (pin 21) indicate interrupt load  

#define fsample  13333.3333             // DAC sample rate in Hz
#define subtone_frequency 123.0         // subtone-frequency to be detected
#define subtone_samples 3000            // number of samples to be processed
#define subtone_sigma 1.0               // standard deviation (typically: 0.2 bis 0.5); here 1 for better frequency resolution   


#endif // CONFIG_H

