/* Copyright (c) 2023, Satvik Reddy <reddy.satvik@gmail.com> */

#include <stdint.h>
#include <stdlib.h>

#include "tft.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"

#define SCL_PIN 2
#define SDA_PIN 3
#define CS_PIN	5
#define DC_PIN	6
#define RST_PIN 7

#define SPI_PORT spi0
#define BAUDRATE (2 * 10 * 1000 * 1000)

#define TFTCMD_NOP     0x00
#define TFTCMD_SWRESET 0x01
#define TFTCMD_SLPOUT  0x11
#define TFTCMD_DISPON  0x29
#define TFTCMD_CASET   0x2A
#define TFTCMD_PASET   0x2B
#define TFTCMD_RAMWR   0x2C
#define TFTCMD_MADCTL  0x36
#define TFTCMD_PIXSET  0x3A

/* TFT internal state. */
typedef struct TFT_state {
	uint16_t x_start;
	uint16_t x_end;
	uint16_t y_start;
	uint16_t y_end;
} TFT_state;

/* Global framebuffer. */
uint16_t *tft_framebuffer;

/* Internal state. */
static TFT_state tft_state;

/* Pulls the CS pin low, indicating a write sequence. */
static inline void tft_cs_select()
{
	asm volatile("nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n");
	gpio_put(CS_PIN, 0);
	asm volatile("nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n");
}

/* Pulls the CS pin high, indicating the end of a write sequence. */
static inline void tft_cs_deselect()
{
	asm volatile("nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n");
	gpio_put(CS_PIN, 1);
	asm volatile("nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n");
}

static inline void tft_command_mode()
{
	gpio_put(DC_PIN, 0);
}

static inline void tft_data_mode()
{
	gpio_put(DC_PIN, 1);
}

static void tft_call_command(uint8_t cmd)
{
	tft_cs_select();
	tft_command_mode();
	spi_write_blocking(SPI_PORT, &cmd, 1);
	tft_data_mode();
	tft_cs_deselect();
}

/* To be used for 1 byte parameters. */
static void tft_add_param8(uint8_t data)
{
	tft_cs_select();
	spi_write_blocking(SPI_PORT, &data, 1);
	tft_cs_deselect();
}

/* Adds data as two 8 bit parameteres, MSB first. */
static void tft_add_param16(uint16_t data)
{
	tft_add_param8((uint8_t)(data >> 8));
	tft_add_param8((uint8_t)data);
}

/**
 * To be used for parameters longer than 1 byte, or sequential
 * data transfer comamnds.
 */
static void tft_write_data(void *buf, size_t len)
{
	tft_cs_select();
	spi_write_blocking(SPI_PORT, buf, len);
	tft_cs_deselect();
}

void tft_init()
{
	/**
	 * 20 MHZ. Calculated from datasheet, though it doesn't really seem to
	 * matter what value I use?
	 */
	spi_init(SPI_PORT, BAUDRATE);
	gpio_set_function(SCL_PIN, GPIO_FUNC_SPI);
	gpio_set_function(SDA_PIN, GPIO_FUNC_SPI);
	gpio_set_function(0, GPIO_FUNC_SPI);

	gpio_init(CS_PIN);
	gpio_set_dir(CS_PIN, GPIO_OUT);
	gpio_put(CS_PIN, 0);

	gpio_init(DC_PIN);
	gpio_set_dir(DC_PIN, GPIO_OUT);
	gpio_put(DC_PIN, 0);

	gpio_init(RST_PIN);
	gpio_set_dir(RST_PIN, GPIO_OUT);
	gpio_put(RST_PIN, 1);

	/* Hardware reset. */
	gpio_put(RST_PIN, 0);
	sleep_ms(10);
	gpio_put(RST_PIN, 1);
	sleep_ms(10);

	/* Software reset. */
	tft_call_command(TFTCMD_SWRESET);

	tft_call_command(TFTCMD_MADCTL);
	tft_add_param8(0xE0); /* X inverted, Y inverted, X-Y swap */

	tft_call_command(TFTCMD_PIXSET);
	tft_add_param8(0x55);

	/* Exit sleep mode. */
	tft_call_command(TFTCMD_SLPOUT);

	/* Turn on display. */
	tft_call_command(TFTCMD_DISPON);

	/* Initialize framebuffer. */
	tft_framebuffer = calloc(TFT_X_LEN * TFT_Y_LEN, sizeof(uint16_t));

	/* Initialize state. */
	tft_set_write_area(0, TFT_X_MAX, 0, TFT_Y_MAX);
}

/* TFT deinitialization. Likely to never actually be run. */
void tft_free()
{
	free(tft_framebuffer);
}

/* Set area to write on the screen. */
void tft_set_write_area(uint16_t x_start, uint16_t x_end, uint16_t y_start,
			uint16_t y_end)
{
	tft_call_command(TFTCMD_CASET);
	tft_add_param16(x_start);
	tft_add_param16(x_end);

	tft_call_command(TFTCMD_PASET);
	tft_add_param16(y_start);
	tft_add_param16(y_end);

	tft_state.x_start = x_start;
	tft_state.y_start = y_start;
	tft_state.x_end = x_end;
	tft_state.y_end = y_end;
}

/* Flush the framebuffer to the screen. */
void tft_flush()
{
	size_t len = (size_t)(tft_state.x_end - tft_state.x_start + 1) *
		     (size_t)(tft_state.y_end - tft_state.x_start + 1);
	tft_call_command(TFTCMD_RAMWR);
	tft_write_data((void *)tft_framebuffer, len * 2);
	tft_call_command(TFTCMD_NOP);
}