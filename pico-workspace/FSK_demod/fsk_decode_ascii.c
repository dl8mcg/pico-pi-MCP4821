/*
*   by dl8mcg Jan. 2025       ASCII - decode
*/

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "fsk_demod.h"
#include "config.h"

static uint8_t rxbit;
static uint8_t bit_count = 0;
static uint8_t bit_buffer = 0;

// Funktionszeiger für Zustandsmaschine
static void state1();
static void state2();
static void state3();
static void state4();

static void (*smAscii)() = state1;      // Initialzustand

void process_ascii(uint8_t bit) 
{
    rxbit = bit;                        // Eingehendes Bit speichern
    smAscii();
}

static void state1()                    // Startbit-Suche
{
    if (rxbit == 0)                     // Startbit erkannt
    {
        bit_count = 0;
        bit_buffer = 0;
        smAscii = state2;
    }
}

static void state2()                    // Datenerfassung
{
    bit_buffer = (bit_buffer >> 1) | (rxbit << 7);  // Bits sammeln (LSB zuerst)
    bit_count++;

    if (bit_count == 8)                 // Alle Datenbits gesammelt
    {
        smAscii = state3;
    }
}

static void state3()                    // Prüfung des ersten Stopp-Bits
{
    if (rxbit == 1)                     // Erstes Stopp-Bit korrekt
    {
        smAscii = state4;
    }
    else                                // Fehler, zurücksetzen
    {
        smAscii = state1;
    }
}

static void state4()                    // Prüfung des zweiten Stopp-Bits
{
    if (rxbit == 1)                     // Zweites Stopp-Bit korrekt
    {
        decoded_char = bit_buffer;      // Dekodiertes Zeichen speichern
        decready = true;                // Signal für fertiges Zeichen
    }
 
    smAscii = state1;                   // Zurücksetzen für nächstes Zeichen
}

