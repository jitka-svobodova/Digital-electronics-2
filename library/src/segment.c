/***********************************************************************
 * Title:    Seven-segment display library
 * Author:   Tomas Fryza, Brno University of Technology, Czechia
 * Software: avr-gcc, tested with avr-gcc 4.9.2
 * Hardware: Any AVR
 *
 * MIT License
 *
 * Copyright (c) 2019 Tomas Fryza
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 **********************************************************************/

/* Includes ----------------------------------------------------------*/
#include <util/delay.h>
#include "gpio.h"
#include "segment.h"

/* Define ------------------------------------------------------------*/
/* Variables ---------------------------------------------------------*/
/* Active low digit 0 to 9 */
uint8_t segment_digit[] = {
   //DPgfedcba
    0b11000000,      // Digit 0
    0b11111001,      // Digit 1
    0b10100100,      // Digit 2
    0b10110000,      // Digit 3
    0b10011001,      // Digit 4
    0b10010010,      // Digit 5
    0b10000010,      // Digit 6
    0b11111000,      // Digit 7
    0b10000000,      // Digit 8
    0b10010000};     // Digit 9

/* Active high position 0 to 3 */
uint8_t segment_position[] = {
    0b00001000,   // Position 0
    0b00000100,   // Position 1
    0b00000010,   // Position 2
    0b00000001};  // Position 3

/* Functions ---------------------------------------------------------*/
void SEG_putc(uint8_t digit,
              uint8_t position)
{
    uint8_t i;

    /* Read values from look-up tables */
    //digit    = segment_digit[digit];
    //position = segment_position[position];
    digit    = segment_digit[digit];
    position = segment_position[position];

    /* Put 1st byte to serial data */
    for (i = 0; i < 8; i++) {
        // TODO: Test and send 8 individual "digit" bits
        //uint8_t digit_masked = digit & (7-i);
        //uint8_t digit_shifted = digit_masked >> (7-i);
        //GPIO_write(&PORTB, SEGMENT_DATA, 0);
        //PIO_write(&PORTB, SEGMENT_DATA, 0)
        //GPIO_write(&PORTB, SEGMENT_DATA, digit_shifted);
        //GPIO_write(&PORTB, SEGMENT_DATA, ((digit & (7-i)) >> (7-i)));
        GPIO_write(&PORTB, SEGMENT_DATA, ((digit & _BV(7-i)) >> (7-i)));
        SEG_toggle_clk();
    }
    /* Put 2nd byte to serial data */
    for (i = 0; i < 8; i++) {
        //uint8_t position_masked = position & (7-i);
        //uint8_t position_shifted = position_masked >> (7-i);
        // TODO: Test and send 8 individual "position" bits
        //GPIO_write(&PORTB, SEGMENT_DATA, 1);
        //GPIO_write(&PORTB, SEGMENT_DATA, position_shifted);
        GPIO_write(&PORTB, SEGMENT_DATA, ((position & _BV(7-i)) >> (7-i)));
        SEG_toggle_clk();
    }

    /* TODO: Generate 1 us latch pulse */
    PORTD ^= _BV(SEGMENT_LATCH);  // toggle the PD4 pin value
    _delay_us(1);   // wait for the delay in ms
    PORTD ^= _BV(SEGMENT_LATCH);  // toggle the PD4 pin value
    //_delay_us(1);   // wait for the delay in ms
}

void SEG_4digits(uint64_t nr2view){
    SEG_putc(nr2view % 10, 0);
    _delay_ms(1);
    SEG_putc((nr2view/10) % 10, 1);
    _delay_ms(1);
    SEG_putc((nr2view/100) % 10, 2);
    //SEG_putc(2, 2);
    _delay_ms(1);
    SEG_putc((nr2view/1000) % 10, 3);
    _delay_ms(1);
};



/*--------------------------------------------------------------------*/
void SEG_toggle_clk(void)
{
    PORTD ^= _BV(SEGMENT_CLK);  // toggle the PD7 pin value
    _delay_us(1);   // wait for the delay in ms
    PORTD ^= _BV(SEGMENT_CLK);  // toggle the PD7 pin value
    _delay_us(1);   // wait for the delay in ms
}
