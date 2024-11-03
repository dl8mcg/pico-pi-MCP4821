/*
*   by dl8mcg Nov. 2024
*/

#pragma once
#ifndef DFT_H
#define DFT_H

void initialize_dft(double frequency, int sample_rate, int number_of_samples);
void process_dft_sample(double sample);
void reset_dft();
double get_dft_amplitude(); // Funktion zum Abrufen der zuletzt berechneten Amplitude

#endif // DFT_H