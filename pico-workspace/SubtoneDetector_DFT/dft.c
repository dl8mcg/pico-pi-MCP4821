/*
*   by dl8mcg Nov. 2024
*/

#include <math.h>
#include "dft.h"

typedef struct 
{
    double cos_omega;
    double sin_omega;
    double real;
    double imag;
    int sample_count;           // samples counter
    int number_of_samples;      // number of samples to be processed
    double amplitude;           // amplitude of subtone signal
} DftState;

static DftState dft_state;      

void initialize_dft(double frequency, int sample_rate, int number_of_samples)
{
    double omega = 2 *  M_PI * frequency / sample_rate;
    dft_state.cos_omega = cos(omega);
    dft_state.sin_omega = sin(omega);
    dft_state.real = 0.0;
    dft_state.imag = 0.0;
    dft_state.sample_count = 0;     
    dft_state.number_of_samples = number_of_samples;
    dft_state.amplitude = 0.0; 
}

void process_dft_sample(double sample)
{
    double real_new = (dft_state.real * dft_state.cos_omega - dft_state.imag * dft_state.sin_omega) + sample;
    double imag_new = (dft_state.real * dft_state.sin_omega + dft_state.imag * dft_state.cos_omega);
    dft_state.real = real_new;
    dft_state.imag = imag_new;
    dft_state.sample_count++; 

    if (dft_state.sample_count >= dft_state.number_of_samples) 
    {
        dft_state.amplitude = sqrt(dft_state.real * dft_state.real + dft_state.imag * dft_state.imag);
        reset_dft();
    }
}

void reset_dft()
{
    dft_state.real = 0.0;
    dft_state.imag = 0.0;
    dft_state.sample_count = 0;
}

double get_dft_amplitude()
{
    return dft_state.amplitude;
}
