/*
 * wifi.c
 *
 *  Created on: Feb 6, 2018
 *      Author: Jingrui
 */

#include <string.h>
#include "wifi.hpp"


/*void Wifi::SendCommand(char command[]) {
	int status;
	status = fwrite(command, strlen(command), 1, Wifi::wifi_uart);
	if (status != 1) {
		printf("wifi: failed to send message, status = %d\n", status);
	}
}*/

void Wifi::LedOn(){
	char command[] = "led_on()\r\n";
	int status = fwrite(command, strlen(command), 1, Wifi::wifi_uart);
	if (status != 1) {
		printf("wifi: failed to send message, status = %d\n", status);
	}
}

void Wifi::LedOff(){
	char command[] = "led_off()\r\n";
	int status = fwrite(command, strlen(command), 1, Wifi::wifi_uart);
	if (status != 1) {
		printf("wifi: failed to send message, status = %d\n", status);
	}
}

void Wifi::LightOn(){
	char command[] = "light_on()\r\n";
	int status = fwrite(command, strlen(command), 1, Wifi::wifi_uart);
	if (status != 1) {
		printf("wifi: failed to send message, status = %d\n", status);
	}
}

void Wifi::LightOff(){
	char command[] = "light_off()\r\n";
	int status = fwrite(command, strlen(command), 1, Wifi::wifi_uart);
	if (status != 1) {
		printf("wifi: failed to send message, status = %d\n", status);
	}
}

Wifi::Wifi(const char name[]) {
	//FILE *wifi_uart = NULL;
	Wifi::wifi_uart = fopen(name, "r+");

	if (wifi_uart == NULL) {
		printf("Failed to open UART device\n");
		while (1)
			;
	}

	//return wifi_uart;
}

