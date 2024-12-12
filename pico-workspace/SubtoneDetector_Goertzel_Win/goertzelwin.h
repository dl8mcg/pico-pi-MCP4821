/*
*   by dl8mcg Nov. 2024
*/

#pragma once
#ifndef DFT_H
#define DFT_H

void initialize_goertzel(float frequency, int sample_rate, int num_samples);
void process_goertzel_sample(float sample);
float get_goertzel_amplitude(); 

#endif // DFT_H