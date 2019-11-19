/*
 * ---------------------------------------------------------------------
 * Author:      Tomas Fryza
 *              Dept. of Radio Electronics, Brno Univ. of Technology
 * Updated by:  Jitka Svobodova
 * Created:     2017-10-27
 * Last update: 2019-11-08
 * Platform:    ATmega328P, 16 MHz, AVR 8-bit Toolchain 3.6.2
 * ---------------------------------------------------------------------
 * Description:
 *    Scan TWI bus for all connected slave devices and transmit info
 *    to UART.
 */

/* Includes ----------------------------------------------------------*/
#include <stdlib.h>         // itoa() function
#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"
#include "uart.h"
#include "twi.h"

/* Typedef -----------------------------------------------------------*/
typedef enum {
    IDLE_STATE = 1,
    TRY_STATE,
    ACK_STATE
} state_t;

/* Define ------------------------------------------------------------*/
#define UART_BAUD_RATE 9600

/* Variables ---------------------------------------------------------*/
state_t current_state = IDLE_STATE;

/* Function prototypes -----------------------------------------------*/

/* Functions ---------------------------------------------------------*/
/**
 *  Brief:  Main program. Test all TWI slave addresses and scan 
 *          connected devices.
 *  Input:  None
 *  Return: None
 */
int main(void)
{
    // UART: asynchronous, 8-bit data, no parity, 1-bit stop
    uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));

    // TWI
    twi_init();

    // Enables interrupts by setting the global interrupt mask
    sei();

    // Put strings to ringbuffer for transmitting via UART.
    uart_puts("\r\n---TEMP read---\r\n");

    /* Timer1
     * Configure Timer1 clock source and enable overflow interrupt */
    TCCR1B = TCCR1B | _BV(CS12);
    TCCR1B = TCCR1B & ~_BV(CS11);
    TCCR1B = TCCR1B | _BV(CS10);
    TIMSK1 = TIMSK1 | _BV(TOIE1);

    // Infinite loop
    for (;;) {
    }
    // Will never reach this
    return (0);
}

/**
 *  Brief: Timer1 overflow interrupt routine. Update state of TWI Finite
 *         State Machine.
 */
ISR(TIMER1_OVF_vect)
{
    char uart_string[3];
    uint8_t read_value;
    // start the write transaction to the temperature sensor ADC at the 5c address
    twi_start((0x5c << 1) + TWI_WRITE);
    // demand the read of the temperature value starting at the register
    // address 0x02
    twi_write(0x02);
    // end the transaction
    twi_stop();

    // initiate the read operation 
    twi_start((0x5c << 1) + TWI_READ);
    read_value = twi_read_ack();
    // print the integer part
    itoa(read_value, uart_string, 16);
    uart_puts(uart_string);
    uart_putc('.');
    // print the fractional part
    read_value = twi_read_nack();
    itoa(read_value, uart_string, 16);
    uart_puts(uart_string);
    uart_puts("°C\r\n");
    // end the transaction
    twi_stop();
}