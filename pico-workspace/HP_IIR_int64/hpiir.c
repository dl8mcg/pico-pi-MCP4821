/*
*   by dl8mcg Dec. 2024
*/

/* High-Pass 300 Hz IIR Filter with Fixed-Point Q24 Arithmetic */

#include <stdint.h>
#include <stdio.h>

#define N 5                // Filter order
#define Q24_SHIFT 24       // Fixed-point shift for Q24 representation

static int64_t acc;

// Filter coefficients (scaled to Q24 format)
int64_t a_q24[N + 1] = 
{    1 << Q24_SHIFT,
    (int64_t)(-4.593307115972737 * (1 << Q24_SHIFT)),
    (int64_t)(8.460407076450299 * (1 << Q24_SHIFT)),
    (int64_t)(-7.804760294104820 * (1 << Q24_SHIFT)),
    (int64_t)(3.602863249809122 * (1 << Q24_SHIFT)),
    (int64_t)(-0.665054482549305 * (1 << Q24_SHIFT))
};

int64_t b_q24[N + 1] = 
{
    (int64_t)(0.821634433543361 * (1 << Q24_SHIFT)),
    (int64_t)(-4.087402974749924 * (1 << Q24_SHIFT)),
    (int64_t)(8.154158701149845 * (1 << Q24_SHIFT)),
    (int64_t)(-8.154158701149845 * (1 << Q24_SHIFT)),
    (int64_t)(4.087402974749925 * (1 << Q24_SHIFT)),
    (int64_t)(-0.821634433543361 * (1 << Q24_SHIFT))
};

// Buffers for input and output values (Q24 format)
int64_t x_q24[N + 1] = {0};     // Input values
int64_t y_q24[N + 1] = {0};     // Output values

// IIR Filter function (Fixed-Point Q24 Arithmetic)
int16_t filter(int16_t input) 
{
    // Scale input to Q24
    int64_t input_q24 = (int64_t)input << (Q24_SHIFT - 12);     // input is from 12 bit - ADC

    // Shift old values in the buffer
    for (int i = N; i > 0; i--) 
    {
        x_q24[i] = x_q24[i - 1];
        y_q24[i] = y_q24[i - 1];
    }

    // Store the current input value
    x_q24[0] = input_q24;

    // Accumulator for filter computation
    acc = 0;

    // Compute the filter output
    for (int i = 0; i <= N; i++) 
    {
        acc += (b_q24[i] * x_q24[i]) >> Q24_SHIFT;              // Contribution from input
        if (i > 0) 
        {
            acc -= (a_q24[i] * y_q24[i]) >> Q24_SHIFT;          // Contribution from output
        }
    }

    // Store the current output value
    y_q24[0] = acc;

    // Scale the output back to 12-bit range
    return (int16_t)(acc >> (Q24_SHIFT - 12));                  // shift back for 12 bit-DAC output
}
