/*
*   by dl8mcg Jan. 2025
*/

#pragma once
#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

#define LED_OB 25                   // GPIO25 for green onboard LED
#define LED_GREEN 7                 // GPIO7 (pin 10) 
#define LED_RED 6                   // GPIO6 (pin 9) 
#define IRQ_LOAD 16                 // GPIO16 (pin 21) interrupt load time 
#define SAMPLING_RATE 13333.3333    // Sampling rate in Hz
//#define SAMPLING_RATE 8800.0      // Sampling rate in Hz

/*
// RTTY settings
#define BAUD_RATE 45.454545         // RTTY Baud rate ham radio
#define TONE_LOW 2125.0             // Low FSK tone in Hz
#define TONE_HIGH 2295.0            // High FSK tone in Hz
#define BIT_DURATION_SAMPLES 294    // Anzahl der Samples für ein Bit (13333 Hz / 45.45 Baud)
#define HALF_BIT_SAMPLES (BIT_DURATION_SAMPLES / 2)
*/

/*// DDK9 10100,8 kHz Pinneberg F1B FSK
#define BAUD_RATE 50.0              // RTTY Baud rate DDK9
#define TONE_LOW 1775.0             // Low FSK tone in Hz
#define TONE_HIGH 2225.0            // High FSK tone in Hz
#define BIT_DURATION_SAMPLES 267    // Anzahl der Samples für ein Bit (13333 Hz / 45.45 Baud)
#define HALF_BIT_SAMPLES (BIT_DURATION_SAMPLES / 2)
*/ 

/*
// ASCII settings
#define BAUD_RATE 300               // ASCII Baud rate 
#define TONE_LOW 1275.0             // Low FSK tone in Hz
#define TONE_HIGH 2125.0            // High FSK tone in Hz
#define BIT_DURATION_SAMPLES (SAMPLING_RATE / BAUD_RATE)  // Samples per bit
#define HALF_BIT_SAMPLES (BIT_DURATION_SAMPLES / 2)
*/


// AX25 settings
#define BAUD_RATE 1200              // AX25 Baud rate
#define TONE_LOW 1200.0             // Low FSK tone in Hz
#define TONE_HIGH 2200.0            // High FSK tone in Hz
#define BIT_DURATION_SAMPLES (SAMPLING_RATE / BAUD_RATE)  // Samples per bit
#define HALF_BIT_SAMPLES (BIT_DURATION_SAMPLES / 2)


#define toggle_OB_LED gpio_xor_mask(1u << LED_OB)
#define init_OB_LED gpio_init(LED_OB);  gpio_set_dir(LED_OB, GPIO_OUT)

extern volatile char decoded_char;  // decoded character
extern volatile bool decready;      // decoded character ready
extern volatile bool demready;      // demodulated bit ready

#endif // CONFIG_H

