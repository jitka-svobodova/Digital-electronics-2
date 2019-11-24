/***********************************************************************
 * 
 * Implementation of LFSR-based (Linear Feedback Shift Register) 
 * pseudo-random generator in AVR assembly.
 * ATmega328P (Arduino Uno), 16 MHz, AVR 8-bit Toolchain 3.6.2
 *
 * Copyright (c) 2017-2019 Tomas Fryza
 * Dept. of Radio Electronics, Brno University of Technology, Czechia
 * This work is licensed under the terms of the MIT license.
 * 
 **********************************************************************/

/* Includes ----------------------------------------------------------*/
#include <stdlib.h>         // itoa() function
#include <avr/io.h>
#include <util/delay.h>

/* Typedef -----------------------------------------------------------*/


/* Define ------------------------------------------------------------*/
#define BURST_DELAY 1

/* Variables ---------------------------------------------------------*/

/* Function prototypes -----------------------------------------------*/
extern uint8_t burst_asm(uint8_t current);

/* Main --------------------------------------------------------------*/
/* Generate a sequence of LFSR preudo-random values using 4- and 8-bit
 * structure. */
int main(void)
{
    // enable the PB5 output
    DDRB |= _BV(PB5); 
    for (;;) {
        // run the burst function for the given number of pulses
        burst_asm(5);
        // wait before next 'transaction'
        _delay_ms(BURST_DELAY);
    }
    return (0);
}

