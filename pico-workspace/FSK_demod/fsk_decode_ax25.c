/*
*   by dl8mcg Jan. 2025       2FSK AX25 - Decoder frame detection
*/

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "fsk_demod.h"
#include "config.h"

// state machine
static void state0();
static void state1();
static void state2();
static void (*smAX25)() = state0;       // function pointer for state machine

static uint8_t rxbyte = 0;              // shift-in register
static uint8_t oldbit = 0;              // old bit
static uint8_t bitcnt = 0;              // bit counter
static uint8_t onecnt = 0;              // counter for one-bits
static bool rxbit = false;              // received bit

void process_ax25(uint8_t bit)
{
    if (bit == oldbit)                  // differential decoding: no change = 1, change = 0
        rxbit = 1; 
    else
        rxbit = 0;           
    oldbit = bit;

    rxbyte = (rxbyte << 1) | rxbit;     // shift in new bit

    if(rxbit == 1)
    {
        onecnt++;
    }
    else
    {
        onecnt = 0;
    }
    if(onecnt > 7)                      // error when more than 7 one-bits
    {
        printf(".");
        onecnt = 0;
        smAX25 = state0;
        return;
    }

    smAX25();
}

void state0()
{
    if (rxbyte == 0x7E)                 // start flag detected
    {
        printf("S");            
        bitcnt = 0;
        smAX25 = state1;        
    }
}

void state1()
{
    bitcnt++;

    if (rxbyte == 0x7E)                 // an additional start flag detected
    {
        printf("W");            
        bitcnt = 0;             
        return;                         // Bleibe in state1
    }

    if (bitcnt < 8)                     // wait for 8 data bits
        return;

    bitcnt = 0;                         // reset bit-counter

    printf("Z");                        // first payload databyte detected
    smAX25 = state2;            
}

void state2()
{
    bitcnt++;

    if (rxbyte == 0x7E)                 // stop flag detected
    {
        printf("E");                    // End-Flag erkannt
        bitcnt = 0;                     // reset bit-counter
        smAX25 = state1;                // back to state1
        return;
    }

    if (bitcnt == 8)                    // payload databyte received
    {
        bitcnt = 0;                     // reset bit-counter
        printf("z");                    
    }
}


