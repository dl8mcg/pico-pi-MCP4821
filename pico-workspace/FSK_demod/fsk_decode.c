/*
*   by dl8mcg Jan. 2025       FSK - decode
*/

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "fsk_demod.h"

#include "config.h"

// baudot - tables
const char letters_table[32] = 
{
    '\0', 'E', '\n', 'A', ' ', 'S', 'I', 'U', 
    '\r', 'D', 'R', 'J', 'N', 'F', 'C', 'K',
    'T', 'Z', 'L', 'W', 'H', 'Y', 'P', 'Q', 
    'O', 'B', 'G', ' ', 'M', 'X', 'V', '\0'
};

const char figures_table[32] = 
{
    '\0', '3', '\n', '-', ' ', '\'', '8', '7', 
    '\r', '$', '4', '\'', ',', '!', ':', '(',
    '5', '+', ')', '2', '#', '6', '0', '1', 
    '9', '?', '&', ' ', '.', '/', '=', '\0'
};

enum RTTY_MODE { LETTERS, FIGURES };

// some variables
static enum RTTY_MODE current_mode = LETTERS;
static uint8_t bit_buffer = 0;
static uint8_t bit_count = 0;

static int stop_bit_count = 0;

// frame synchronisation indication
static bool in_frame = false;                           // is a valid bit-sequence?

volatile char decoded_char = 0;

// function processig one data bit
void process_bit_rtty(uint8_t bit) 
{
    if (!in_frame) 
    {
        // wait for start-bit (low)
        if (bit == 0) 
        {
            in_frame = true;
            bit_buffer = 0;
            bit_count = 0;
        }
    } 
    else 
    {
        // collect 5 data-bits
        if (bit_count < 5) 
        {
            bit_buffer = (bit_buffer >> 1) | (bit << 4);            // MSB first
            bit_count++;
        } 
        else if (bit == 1) 
        {
            // validate stop-bit (high)
            const char* table = (current_mode == LETTERS) ? letters_table : figures_table;
            if (bit_buffer == 0b11111)  // change to Letters
            {  
                current_mode = LETTERS;
            } 
            else if (bit_buffer == 0b11011)  // change to Figures
            {  
                current_mode = FIGURES;
            } 
            else 
            {
                decoded_char = table[bit_buffer & 0x1F];  // 5 lsbs
            }

            // reset frame
            in_frame = false;
        } 
        else 
        {
            // reset faulty stop bit sequence
            in_frame = false;
        }
    }
}


// Function to process one bit of data
void process_bit_ascii(uint8_t bit) 
{
    if (!in_frame) 
    {
        // Wait for start bit (low)
        if (bit == 0) 
        {
            in_frame = true;
            bit_buffer = 0;
            bit_count = 0;
            stop_bit_count = 0;
        }
    } 
    else 
    {
        if (bit_count < DATA_BITS) 
        {
            // Collect data bits (LSB first)
            bit_buffer = (bit_buffer >> 1) | (bit << 7);
            bit_count++;
        } 
        else if (stop_bit_count < STOP_BITS) 
        {
            // Validate stop bits (high)
            if (bit == 1) 
            {
                stop_bit_count++;
                if (stop_bit_count == STOP_BITS) 
                {
                    // Frame complete, decode character
                    decoded_char = bit_buffer;
                    in_frame = false;
                }
            } 
            else 
            {
                // Invalid stop bit sequence, reset frame
                in_frame = false;
            }
        }
    }
}



char get_decoded_FSK()
{
    if(demod_bit)
    {
        demod_bit &= 0x01;
        //process_bit_rtty(demod_bit);      // demodulated bit from fsk_demod.c (Timer-IRQ) to FSK_decode.c (main program)
        process_bit_ascii(demod_bit);      // demodulated bit from fsk_demod.c (Timer-IRQ) to FSK_decode.c (main program)
        demod_bit = 0;
    } 

    char character = decoded_char;
    decoded_char = 0;
    return character;                           // output decoded character, 0 is indicating no decoded character
}