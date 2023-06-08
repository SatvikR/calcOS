/* Copyright (c) 2023, Satvik Reddy <reddy.satvik@gmail.com> */

#include <stdio.h>

#include "pico/stdlib.h"
#include "tft.h"

int main()
{
	/* Currently just a test program that draws some shapes using the
   TFT api. */
	tft_init();
	tft_flush();

	/* Draw a black background. */
	for (int i = 0; i < (TFT_X_LEN * TFT_Y_LEN); i++) {
		tft_framebuffer[i] = TFT_BLACK;
	}
	tft_flush();

	/* Draw a centered white rectangle. */
	tft_set_write_area(50, TFT_X_MAX - 50, 50, TFT_Y_MAX - 50);
	int len = (TFT_X_LEN - 100) * (TFT_Y_LEN - 100);
	for (int i = 0; i < len; i++) {
		tft_framebuffer[i] = TFT_WHITE;
	}
	tft_flush();

	/* Draw a smaller centered black rectangle. */
	tft_set_write_area(80, TFT_X_MAX - 80, 80, TFT_Y_MAX - 80);
	len = (TFT_X_LEN - 160) * (TFT_Y_LEN - 160);
	for (int i = 0; i < len; i++) {
		tft_framebuffer[i] = TFT_BLACK;
	}
	tft_flush();

	/* Draw a smaller centered white rectangle. */
	tft_set_write_area(100, TFT_X_MAX - 100, 100, TFT_Y_MAX - 100);
	len = (TFT_X_LEN - 200) * (TFT_Y_LEN - 200);
	for (int i = 0; i < len; i++) {
		tft_framebuffer[i] = TFT_WHITE;
	}
	tft_flush();

	/* idle. */
	gpio_init(PICO_DEFAULT_LED_PIN);
	gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
	while (true) {
		gpio_put(PICO_DEFAULT_LED_PIN, 1);
		sleep_ms(250);
		gpio_put(PICO_DEFAULT_LED_PIN, 0);
		sleep_ms(250);
	}

	tft_free();
	return 0;
}