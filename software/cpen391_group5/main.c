#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "system.h"
#include "sys/alt_alarm.h"
#include "simple_graphics.h"
#include "vision.h"
#include "wifi.h"

int main(int argc, const char * argv[]) {
	/*struct point prev, cur;

	 while(1) {
	 //unsigned start = alt_nticks();
	 find_dots(&cur);
	 //printf("Algorithm took %lu ms\n", 1000 * (alt_nticks() - start) / alt_ticks_per_second());
	 gesture_detect(&prev, &cur);
	 memcpy(&prev, &cur, sizeof(struct point));
	 }*/

	FILE *wifi_uart = NULL;
	wifi_uart = WIFIInit(WIFI_UART_NAME);
	printf("start\n");
	int input;

	while (1) {
		input = -1;
		printf("1 = led_on, 0 = led_off\n");
		scanf("%d", &input);
		if (input == 1) {
			printf("led_on\n");
			WIFISendCommand(LED_ON, wifi_uart);
		} else if (input == 0) {
			printf("led off\n");
			WIFISendCommand(LED_OFF, wifi_uart);
		} else {
			printf("unrecognized command\n");
		}
	}

	return 0;
}
