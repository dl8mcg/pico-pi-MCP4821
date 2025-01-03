/*
*   by dl8mcg Jan. 2025
*/

#pragma once
#ifndef DCALLT_H
#define DCALLT_H

extern volatile int demod_bit;

void initialize_fsk();
void process_fsk_demodulation(int16_t sample); 

#endif // DCALLT_H