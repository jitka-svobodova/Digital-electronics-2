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
void fsm_twi_scanner(void);

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
    uart_puts("\r\n---TWI scanner---");
    uart_puts("\r\n     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f");

    /* Timer1
     * Configure Timer1 clock source and enable overflow interrupt */
    TCCR1B = TCCR1B & ~_BV(CS12);
    TCCR1B = TCCR1B | _BV(CS11);
    TCCR1B = TCCR1B & ~_BV(CS10);
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
    fsm_twi_scanner();
}

/**
 *  Brief: Test one TWI address.
 */
void fsm_twi_scanner(void)
{
    static uint8_t addr = 0;
    uint8_t status;
    char uart_string[3];

    switch (current_state) {
    case IDLE_STATE:
        if (addr < 128) {                          // scan addresses range 0 to 127
            if (addr % 16 == 0) {                  // show 16 values per row
                uart_puts("\r\n");
                itoa(addr, uart_string, 16);
                uart_puts(uart_string);
                if (addr == 0) {                  // add a zero for proper
                    uart_putc('0');               // formatting of the address 0
                }
                uart_putc(':');
            }
            current_state = TRY_STATE;
        }
        break;

    // Transmit TWI slave address and check status
    case TRY_STATE:
        status = twi_start((addr<<1) + TWI_WRITE); // 7 LSBs of an address + W
        twi_stop();

        if (status == 0) {
            current_state = ACK_STATE;
        } else {
            addr++;
            current_state = IDLE_STATE;
            if (addr < 8 || addr >= 120)           // indicate reserved addresses
            {
                uart_puts("\033[31m RA\033[0m");
            }
            else {
                uart_puts(" --");                  // indicate non-occupied addresses
            }
        }
        break;

    // Slave ACK received
    case ACK_STATE:
        uart_puts("\033[32m OK\033[0m"); // indicate occupied addresses
        addr++;
        current_state = IDLE_STATE;
        break;

    default:
        current_state = IDLE_STATE;
    }
}
