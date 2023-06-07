#include <stdio.h>
#include "pico/stdlib.h"
#include "tft.h"

int main() {
	stdio_init_all();
	tft_init();
	gpio_init(PICO_DEFAULT_LED_PIN);
    	gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
	while (true) {
		tft_test();
	}
	return 0;
}