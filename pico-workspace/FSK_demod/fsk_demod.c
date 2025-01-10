/*
*   by dl8mcg Jan. 2025  2FSK - demodulator
*/

#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "fsk_demod.h"
#include "config.h"
#include "pico/stdlib.h"
#include "hardware/sync.h"

// NCO parameters
volatile float nco_phase_low = 0.0f;
volatile float nco_phase_high = 0.0f;
const float nco_step_low = 2 * M_PI * TONE_LOW / SAMPLING_RATE;
const float nco_step_high = 2 * M_PI * TONE_HIGH / SAMPLING_RATE;

// Filter coefficients (IIR low-pass filter)
//#define LPF_ALPHA 0.1f
//#define LPF_ALPHA 0.075f
#define LPF_ALPHA 0.070f
volatile float i_low = 0, q_low = 0;
volatile float i_high = 0, q_high = 0;

// Synchronization variables
volatile int bit_count = 0;
volatile float amplitude_low = 0;
volatile float amplitude_high = 0;
volatile int bit_value = -1;
volatile int previous_bit_value = -1;
volatile float bit_timer = 0;

volatile int demod_bit = 0;

volatile bool demready = false;

void led_signaling(int led_bit);

void process_fsk_demodulation(int16_t raw_sample)
{
    float sample = (float)raw_sample / 4096.0f - 0.5f;                  // Normalize ADC value to [-0.5, 0.5]

    // Update NCO for low and high tones
    nco_phase_low += nco_step_low;
    if (nco_phase_low > 2 * M_PI) nco_phase_low -= 2 * M_PI;
        
    nco_phase_high += nco_step_high;
    if (nco_phase_high > 2 * M_PI) nco_phase_high -= 2 * M_PI;

    // convert FSK signal down to DC with NCOs for high and low tone
    float i_sample_low = sample * cosf(nco_phase_low);
    float q_sample_low = sample * sinf(nco_phase_low);
    float i_sample_high = sample * cosf(nco_phase_high);
    float q_sample_high = sample * sinf(nco_phase_high);

    // Apply low-pass filter to I and Q components  IIR-Filter
    i_low += LPF_ALPHA * (i_sample_low - i_low);
    q_low += LPF_ALPHA * (q_sample_low - q_low);
    i_high += LPF_ALPHA * (i_sample_high - i_high);
    q_high += LPF_ALPHA * (q_sample_high - q_high);

    // Compute amplitudes for low- and high - tones
    amplitude_low = sqrtf(i_low * i_low + q_low * q_low);
    amplitude_high = sqrtf(i_high * i_high + q_high * q_high);

    // Detect bit based on amplitude comparison
    bit_value = (amplitude_high > amplitude_low) ? 1 : 0;

    // Edge detection for synchronisation
    if (bit_value != previous_bit_value) 
    {
        // Synchronisation on the edge
        bit_timer = HALF_BIT_SAMPLES;
        previous_bit_value = bit_value;
    }

    // bit sampling
    if (--bit_timer <= 0) 
    {
        // sample bit and provide
        demod_bit = bit_value;                       // bit 7 in demod_bit indicating new bit sampled
        demready = true;

        // reset timer for next bit - sampling
        bit_timer = BIT_DURATION_SAMPLES;

        led_signaling(bit_value);
    }
}

void initialize_fsk() 
{
    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    gpio_init(LED_RED);
    gpio_set_dir(LED_RED, GPIO_OUT);
}

void led_signaling(int led_bit)
{
    if(led_bit == 1)
    {
        gpio_put(LED_RED, 1); 
        gpio_put(LED_GREEN, 0); 
    }
    else    
    {
        gpio_put(LED_RED, 0); 
        gpio_put(LED_GREEN, 1);             
    }
}



