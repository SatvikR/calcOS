/* Copyright (c) 2023, Satvik Reddy <reddy.satvik@gmail.com> */

#ifndef _TFT_H
#define _TFT_H

#include <stdint.h>

#define TFT_X_MAX 319
#define TFT_Y_MAX 239
#define TFT_X_LEN 320
#define TFT_Y_LEN 240

/* Macros to translate between x/y and framebuffer index. */
#define TFT_GET_X(i)	(i % TFT_X_LEN)
#define TFT_GET_Y(i)	(i / TFT_X_LEN)
#define TFT_GET_I(x, y) (x + y * TFT_X_LEN)

#define TFT_WHITE 0xFFFF
#define TFT_BLACK 0x0000

extern uint16_t *tft_framebuffer;

/* Initialize TFT */
void tft_init();

/* TFT deinitialization. Likely to never actually be run. */
void tft_free();

/* Set area to write on the screen. */
void tft_set_write_area(uint16_t x_start, uint16_t x_end, uint16_t y_start,
			uint16_t y_end);

/* Flush the framebuffer to the screen. */
void tft_flush();

#endif