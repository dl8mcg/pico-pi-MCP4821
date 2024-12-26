/*
*   by dl8mcg Dec. 2024
*/

#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "detect_subtone.h"
#include "config.h"
#include "pico/stdlib.h"

#define Q24_SHIFT 24
#define Q24_SCALE (1 << Q24_SHIFT)

volatile static int64_t* window_q24 = NULL;                   // Gauss window function (Q24 format)

volatile static int64_t coefficient_q24;
volatile static int64_t sine_q24;
volatile static int64_t cosine_q24;

volatile static int64_t q0_q24, q1_q24, q2_q24;
volatile static int64_t real_q24;
volatile static int64_t imag_q24;
volatile static int64_t q1res_q24;
volatile static int64_t q2res_q24;

volatile static int64_t real_sq;
volatile static int64_t imag_sq;
volatile static int64_t magnitude_sq;

volatile static int number_of_samples;
volatile static int sample_count;
volatile static float amplitude;

static int64_t calc_subdet_amplitude();
static void reset_subdet();

void initialize_subdet() 
{
    number_of_samples = subtone_samples;
    window_q24 = (int64_t*)malloc(number_of_samples * sizeof(int64_t));   // Allocate memory for window function coefficients

    // Gauss window
    double N_minus_1 = number_of_samples - 1;
    double alpha = N_minus_1 / 2.0;                                       // Center of window

    for (int n = 0; n < number_of_samples; n++) 
    {
        double normalized_n = (n - alpha) / (subtone_sigma * alpha);
        double value = exp(-0.5 * normalized_n * normalized_n);
        window_q24[n] = (int64_t)(value * Q24_SCALE);
    }

    // Initialize Goertzel coefficients
    double omega = (2.0 * M_PI * subtone_frequency) / fsample;
    double cosine = cos(omega);
    double sine = sin(omega);
    
    coefficient_q24 = (int64_t)(2.0 * cosine * Q24_SCALE);
    cosine_q24 = (int64_t)(cosine * Q24_SCALE);
    sine_q24 = (int64_t)(sine * Q24_SCALE);

    gpio_init(SUB_DET_LED);
    gpio_set_dir(SUB_DET_LED, GPIO_OUT);
}

void process_subdet_sample(int32_t sample)
{
    // Apply Goertzel algorithm with fixed-point arithmetic
    q0_q24 = (coefficient_q24 * q1_q24 >> Q24_SHIFT) - q2_q24 + ((int64_t)sample * window_q24[sample_count] >> Q24_SHIFT);
    q2_q24 = q1_q24;
    q1_q24 = q0_q24;

    sample_count++;

    if (sample_count >= number_of_samples)
    {
        q1res_q24 = q1_q24;
        q2res_q24 = q2_q24;
        reset_subdet();
    }
}

static void reset_subdet()
{
    q0_q24 = 0;
    q1_q24 = 0;
    q2_q24 = 0;
    sample_count = 0;
}

static int64_t calc_subdet_amplitude()
{
    // Calculate real and imaginary components
    real_q24 = q1res_q24 - ((q2res_q24 * cosine_q24) >> Q24_SHIFT);
    imag_q24 = (q2res_q24 * sine_q24) >> Q24_SHIFT;

    // Calculate amplitude
    real_sq = (real_q24 * real_q24) >> Q24_SHIFT;
    imag_sq = (imag_q24 * imag_q24) >> Q24_SHIFT;
    magnitude_sq = real_sq + imag_sq;

    // square root 
    return (int64_t)(sqrt((double)magnitude_sq) * Q24_SCALE / number_of_samples);
}

float get_subdet_amplitude()
{
    amplitude = (float) calc_subdet_amplitude() / 1100000.0f;          // amplitude in V
    return (amplitude);
}
