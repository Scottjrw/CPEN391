/*
 * wifi.c
 *
 *  Created on: Feb 6, 2018
 *      Author: Jingrui
 */

#include "WIFI.h"

void WIFISendCommand(char command[], FILE *f) {
	int status;
	status = fwrite(command, strlen(command), 1, f);
	if (status != 1) {
		printf("wifi: failed to send message, status = %d\n", status);
	}
}

FILE* WIFIInit(char name[]) {
	FILE *wifi_uart = NULL;
	wifi_uart = fopen(name, "r+");

	if (wifi_uart == NULL) {
		printf("Failed to open UART device\n");
		while (1)
			;
	}

	return wifi_uart;
}

