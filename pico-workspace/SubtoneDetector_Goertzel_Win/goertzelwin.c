/*
*   by dl8mcg Nov. 2024
*/

#include <stdlib.h>
#include <math.h>
#include "goertzelwin.h"

static float* window = NULL;                   // window function

static float sine;
static float cosine;
static float coefficient;
static float q0, q1, q2;
static int number_of_samples;
static int sample_count;
static float amplitude;

static float q1res;
static float q2res;


static float calc_goertzel_amplitude();
static void reset_goertzel();

void initialize_goertzel(float target_freq, int sample_rate, int num_samples) 
{
    number_of_samples = num_samples;
    window = (float*)malloc(number_of_samples * sizeof(float));   // allocate memory for window function coefficients 

    // gauss_window
    float sigma = 1.0;                         // standard deviation (typically: 0.2 bis 0.5); here 1 for better frequency resolution     
    float N_minus_1 = number_of_samples - 1;   // N-1 for normalization
    float alpha = (N_minus_1) / 2.0;           // center of window

    for (int n = 0; n < number_of_samples; n++)
    {
        float normalized_n = (n - alpha) / (sigma * alpha);
        window[n] = exp(-0.5 * normalized_n * normalized_n);
    } 

    // init goertzel 
    float omega = (2.0 * M_PI * target_freq) / sample_rate;
    sine = sin(omega);
    cosine = cos(omega);
    coefficient = 2.0 * cosine;
}

void process_goertzel_sample(float sample)
{
    q0 = coefficient * q1 - q2 + sample * window[sample_count];
    q2 = q1;
    q1 = q0;

    sample_count++; 

    if (sample_count >= number_of_samples)
    {
        q1res = q1;
        q2res = q2;

        reset_goertzel();
    }
}

static void reset_goertzel()
{
    q0 = 0.0;
    q1 = 0.0;
    q2 = 0.0;
    sample_count = 0;
}

static float calc_goertzel_amplitude()
{
    float real = (q1res - q2res * cosine);
    float imag = (q2res * sine);
    return sqrt(real * real + imag * imag) * 7.6 / number_of_samples;
}

float get_goertzel_amplitude()
{
    amplitude = calc_goertzel_amplitude();
    return amplitude;
}