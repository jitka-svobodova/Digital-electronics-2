;* ---------------------------------------------------------------------
;*
;* Assembly implementation of 4- and 8--bit LFSR-based (Linear Feedback
;* Shift Register) pseudo-random generators.
;* ATmega328P (Arduino Uno), 16 MHz, AVR 8-bit Toolchain 3.6.2
;*
;* Copyright (c) 2017-2019 Tomas Fryza
;* Dept. of Radio Electronics, Brno University of Technology, Czechia
;* This work is licensed under the terms of the MIT license.
;*
;* https://www.xilinx.com/support/documentation/application_notes/xapp052.pdf
;* http://courses.cse.tamu.edu/walker/csce680/lfsr_table.pdf
;*
;* ---------------------------------------------------------------------

;* Includes ------------------------------------------------------------
; Set offset for control register addresses (NEEDED FOR I/O REGISTERS)
#define __SFR_OFFSET    0
#include <avr/io.h>

;* Define --------------------------------------------------------------
#define input   r24

;* Functions -----------------------------------------------------------
;* Pulse burst
;* Input:  r24 - Current value of shift register
.global burst_asm
burst_asm:
    loop: dec input  ; decrease input by 1 (sets Z='1', if input value is zero)
    sbi PORTB, 5     ; set bit PB5 to '1'
    cbi PORTB, 5     ; reset bit PB5 to '0'
    brne loop        ; if Z=='0', go to 'loop'
    ret              ; return from subroutine