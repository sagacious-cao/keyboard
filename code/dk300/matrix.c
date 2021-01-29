/*
Copyright 2012 Jun Wako <wakojun@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ch.h"
#include "hal.h"

#include "config.h"

/*
 * scan matrix
 */
#include "print.h"
#include "debug.h"
#include "util.h"
#include "matrix.h"
#include "wait.h"

#ifndef DEBOUNCE
    #define DEBOUNCE 5
#endif
static uint8_t debouncing = DEBOUNCE;

/* matrix state(1:on, 0:off) */
static matrix_row_t matrix[MATRIX_ROWS];
static matrix_row_t matrix_debouncing[MATRIX_ROWS];

static matrix_row_t read_cols(void);
static void init_cols(void);
static void unselect_rows(void);
static void select_row(uint8_t row);


inline
uint8_t matrix_rows(void)
{
    return MATRIX_ROWS;
}

inline
uint8_t matrix_cols(void)
{
    return MATRIX_COLS;
}

/* generic STM32F103C8T6 board */
#ifdef BOARD_GENERIC_STM32_F103
#define LED_ON()    do { palClearPad(GPIOC, GPIOC_LED) ;} while (0)
#define LED_OFF()   do { palSetPad(GPIOC, GPIOC_LED); } while (0)
#define LED_TGL()   do { palTogglePad(GPIOC, GPIOC_LED); } while (0)
#endif

/* Maple Mini */
#ifdef BOARD_MAPLEMINI_STM32_F103
#define LED_ON()    do { palSetPad(GPIOB, 1) ;} while (0)
#define LED_OFF()   do { palClearPad(GPIOB, 1); } while (0)
#define LED_TGL()   do { palTogglePad(GPIOB, 1); } while (0)
#endif

void matrix_init(void)
{
    // initialize row and col
    unselect_rows();
    init_cols();

    // initialize matrix state: all keys off
    for (uint8_t i=0; i < MATRIX_ROWS; i++) {
        matrix[i] = 0;
        matrix_debouncing[i] = 0;
    }

    //debug
    debug_matrix = true;
    LED_ON();
    wait_ms(500);
    LED_OFF();
}

uint8_t matrix_scan(void)
{
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) 
    {
        select_row(i);
        wait_us(30);  // without this wait read unstable value.
        matrix_row_t cols = read_cols();
        if (matrix_debouncing[i] != cols) 
        {
            matrix_debouncing[i] = cols;
            if (debouncing) {
                debug("bounce!: "); 
                debug_hex(debouncing); 
                debug("\n");
            }
            debouncing = DEBOUNCE;
        }
        unselect_rows();
    }

    if (debouncing) 
    {
        if (--debouncing) 
        {
            wait_ms(1);
        } 
        else 
        {
            for (uint8_t i = 0; i < MATRIX_ROWS; i++) 
            {
                matrix[i] = matrix_debouncing[i];
            }
        }
    }

    return 1;
}

inline
bool matrix_is_on(uint8_t row, uint8_t col)
{
    return (matrix[row] & ((matrix_row_t)1<<col));
}

inline
matrix_row_t matrix_get_row(uint8_t row)
{
    return matrix[row];
}

void matrix_print(void)
{
    print("\nr/c 0123456789ABCDEF\n");
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        phex(row); print(": ");
        pbin_reverse16(matrix_get_row(row));
        print("\n");
    }
}

/* Column pin configuration
 */
static void  init_cols(void)
{
    int i;

    /* 
    for (i = 0; i <= 7; i++) {
        palSetPadMode(GPIOA, i, PAL_MODE_INPUT_PULLUP);
    }
    palSetPadMode(GPIOB, 0, PAL_MODE_INPUT_PULLUP);
    */
    for (i = 8; i <= 10; i++) {
        palSetPadMode(GPIOA, i, PAL_MODE_INPUT_PULLDOWN);
    }
    palSetPadMode(GPIOA, 15, PAL_MODE_INPUT_PULLDOWN);
    for (i = 3; i <= 15; i++) {
        palSetPadMode(GPIOB, i, PAL_MODE_INPUT_PULLDOWN);
    }
    palSetPadMode(GPIOB, 0, PAL_MODE_INPUT_PULLDOWN);
    palSetPadMode(GPIOB, 1, PAL_MODE_INPUT_PULLDOWN);
    palSetPadMode(GPIOC, 15, PAL_MODE_INPUT_PULLDOWN);
}

/* Returns status of switches(1:on, 0:off) */
static matrix_row_t read_cols(void)
{
    matrix_row_t tmp = 0;
    /*
    tmp  = (~palReadPort(GPIOA)) & 0xff;    
    tmp |= (palReadPad(GPIOB, 0) == PAL_LOW) ? (1 << 8) : 0;
    */
    tmp |= (palReadPad(GPIOA, 8)==PAL_HIGH) ? (1<<0):0;
    tmp |= (palReadPad(GPIOA, 9)==PAL_HIGH) ? (1<<1):0;
    tmp |= (palReadPad(GPIOA, 10)==PAL_HIGH) ? (1<<2):0;
    tmp |= (palReadPad(GPIOA, 15)==PAL_HIGH) ? (1<<3):0;
    tmp |= (palReadPad(GPIOB, 0)==PAL_HIGH) ? (1<<4):0;
    tmp |= (palReadPad(GPIOB, 1)==PAL_HIGH) ? (1<<5):0;
    tmp |= (palReadPad(GPIOB, 3)==PAL_HIGH) ? (1<<6):0;
    tmp |= (palReadPad(GPIOB, 4)==PAL_HIGH) ? (1<<7):0;
    tmp |= (palReadPad(GPIOB, 5)==PAL_HIGH) ? (1<<8):0;
    tmp |= (palReadPad(GPIOB, 6)==PAL_HIGH) ? (1<<9):0;
    tmp |= (palReadPad(GPIOB, 7)==PAL_HIGH) ? (1<<10):0;
    tmp |= (palReadPad(GPIOB, 8)==PAL_HIGH) ? (1<<11):0;
    tmp |= (palReadPad(GPIOB, 9)==PAL_HIGH) ? (1<<12):0;
    tmp |= (palReadPad(GPIOB, 10)==PAL_HIGH) ? (1<<13):0;
    tmp |= (palReadPad(GPIOB, 11)==PAL_HIGH) ? (1<<14):0;
    tmp |= (palReadPad(GPIOB, 12)==PAL_HIGH) ? (1<<15):0;
    tmp |= (palReadPad(GPIOB, 13)==PAL_HIGH) ? (1<<16):0;
    tmp |= (palReadPad(GPIOB, 14)==PAL_HIGH) ? (1<<17):0;
    tmp |= (palReadPad(GPIOB, 15)==PAL_HIGH) ? (1<<18):0;
    tmp |= (palReadPad(GPIOC, 15)==PAL_HIGH) ? (1<<19):0;
    return tmp;
}

/* Row pin configuration
 */
static void unselect_rows(void)
{
  
    palSetPadMode(GPIOA, 0, PAL_MODE_INPUT); // hi-Z
    palSetPadMode(GPIOA, 1, PAL_MODE_INPUT); // hi-Z
    palSetPadMode(GPIOA, 2, PAL_MODE_INPUT); // hi-Z
    palSetPadMode(GPIOA, 3, PAL_MODE_INPUT); // hi-Z
    palSetPadMode(GPIOA, 4, PAL_MODE_INPUT); // hi-Z
    palSetPadMode(GPIOA, 5, PAL_MODE_INPUT); // hi-Z
    palSetPadMode(GPIOA, 6, PAL_MODE_INPUT); // hi-Z
    palSetPadMode(GPIOA, 7, PAL_MODE_INPUT); // hi-Z
  
   /*
    palClearPad(GPIOA, 0);
    palClearPad(GPIOA, 1);
    p6alClearPad(GPIOA, 2);
    palClearPad(GPIOA, 3);
    palClearPad(GPIOA, 4);
    palClearPad(GPIOA, 5);
    palClearPad(GPIOA, 6);
    palClearPad(GPIOA, 7);
*/
}

static void select_row(uint8_t row)
{
    //(void)row;
    // Output low to select
    switch (row) {
        case 0:
            palSetPadMode(GPIOA, 0, PAL_MODE_OUTPUT_PUSHPULL);
            palSetPad(GPIOA, 0);
            break;
        case 1:
            palSetPadMode(GPIOA, 1, PAL_MODE_OUTPUT_PUSHPULL);
            palSetPad(GPIOA, 1);
            break;
        case 2:
            palSetPadMode(GPIOA, 2, PAL_MODE_OUTPUT_PUSHPULL);
            palSetPad(GPIOA, 2);
            break;                        
        case 3:
            palSetPadMode(GPIOA, 3, PAL_MODE_OUTPUT_PUSHPULL);
            palSetPad(GPIOA, 3);
            break;
        case 4:
            palSetPadMode(GPIOA, 4, PAL_MODE_OUTPUT_PUSHPULL);
            palSetPad(GPIOA, 4);
            break;
        case 5:
            palSetPadMode(GPIOA, 5, PAL_MODE_OUTPUT_PUSHPULL);
            palSetPad(GPIOA, 5);
            break;
        case 6:
            palSetPadMode(GPIOA, 6, PAL_MODE_OUTPUT_PUSHPULL);
            palSetPad(GPIOA, 6);
            break;
        case 7:
            palSetPadMode(GPIOA, 7, PAL_MODE_OUTPUT_PUSHPULL);
            palSetPad(GPIOA, 7);
            break;
    }
}
