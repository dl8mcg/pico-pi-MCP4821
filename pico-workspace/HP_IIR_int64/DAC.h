/*
*   by dl8mcg Dec. 2024
*/

#pragma once
#ifndef DAC_H
#define DAC_H

#include <stdint.h>

#define LED_OB 25               // Define onboard LED pin for toggle in DAC logic

void initialize_spi();
void spi_write(uint16_t data);

#endif // DAC_H
