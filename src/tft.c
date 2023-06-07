/* TFT.C --
	Provides a low level abstraction to interact with the
	ILI9341 TFT display.
*/

#include <stdint.h>
#include "hardware/spi.h"
#include "pico/stdlib.h"
#include "tft.h"

#define SCL_PIN 2
#define SDA_PIN 3
#define CS_PIN  5
#define DC_PIN  6
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

/* Pulls the CS pin low, indicating a write sequence. */
static inline void tft_cs_select() {
	asm volatile("nop \n nop \n nop");
	gpio_put(CS_PIN, 0);
	asm volatile("nop \n nop \n nop");
}

/* Pulls the CS pin high, indicating the end of a write sequence. */
static inline void tft_cs_deselect() {
	asm volatile("nop \n nop \n nop");
	gpio_put(CS_PIN, 1);
	asm volatile("nop \n nop \n nop");
}

static inline void tft_command_mode() {
	gpio_put(DC_PIN, 0);
}

static inline void tft_data_mode() {
	gpio_put(DC_PIN, 1);
}

static void tft_call_command(uint8_t cmd) {
	tft_cs_select();
	tft_command_mode();
	spi_write_blocking(SPI_PORT, &cmd, 1);
	tft_data_mode();
	tft_cs_deselect();
}

/* To be used for 1 byte parameters. */
static void tft_add_param8(uint8_t data) {
	tft_cs_select();
	spi_write_blocking(SPI_PORT, &data, 1);
	tft_cs_deselect();
}

/**
 * To be used for parameters longer than 1 byte, or sequential
 * data transfer comamnds.
*/
static void tft_write_data(void *buf, size_t len) {
	tft_cs_select();
	// tft_data_mode();
	spi_write_blocking(SPI_PORT, buf, len);
	tft_cs_deselect();
}

void tft_init() {
	/* 2 MHZ */
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
	tft_add_param8(0x14); /* 00010100 */

	tft_call_command(TFTCMD_PIXSET);
	tft_add_param8(0x55);


	/* Exit sleep mode. */
	tft_call_command(TFTCMD_SLPOUT);

	/* Turn on display. */
	tft_call_command(TFTCMD_DISPON);
}

void tft_test() {
	tft_call_command(TFTCMD_CASET);
	tft_add_param8(0);
	tft_add_param8(0);
	tft_add_param8(0);
	tft_add_param8(0xEF);

	tft_call_command(TFTCMD_PASET);
	tft_add_param8(0);
	tft_add_param8(0);
	tft_add_param8(0x01);
	tft_add_param8(0x3F);

	uint16_t buf[240 * 320];
	uint16_t color;
	int i;
	int row;
	for (i = 0; i < 240 * 320; i++) {
		row = i / 240;
		if (i < 240 * 320 / 4) {
			color = 0xF800;
		} else if (i < 240 * 320 / 4 * 2) {
			color = 0xFD20;
		} else if (i < 240 * 320 / 4 * 3) {
			color = 0xFFE0;
		} else {
			color = 0x07E0;
		}
		buf[i] = color;
	}

	tft_call_command(TFTCMD_RAMWR);
	tft_write_data((void *)buf, 240 * 320 * 2);

	gpio_put(PICO_DEFAULT_LED_PIN, 1);
	sleep_ms(250);
	gpio_put(PICO_DEFAULT_LED_PIN, 0);
	sleep_ms(250);
}