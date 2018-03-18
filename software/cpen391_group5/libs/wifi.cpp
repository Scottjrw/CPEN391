/*
 * wifi.c
 *
 *  Created on: Feb 6, 2018
 *      Author: Jingrui
 */

#include <string.h>
#include "wifi.hpp"
#include <iostream>


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

void Wifi::SendUsername(std::string username, std::string password){
	std::string s= "send_username(\""+username+"\",\""+password+"\")\r\n";
	const char * command = s.c_str();
	int status = fwrite(command, strlen(command), 1, wifi_uart);
	if(status!=1){
		printf("wifi: failed to send message, status = %d\n", status);
	}
}

void Wifi::SendPicture(std::string picture){
	std::string s= "send_picture(\""+picture+"\")\r\n";
	const char * command = s.c_str();
	int status = fwrite(command, strlen(command), 1, wifi_uart);
	if(status!=1){
		printf("wifi: failed to send message, status = %d\n", status);
	}
}
std::string Wifi::ReadResponse(){
	std::string response;

	while(1){
		char temp = fgetc(wifi_uart);
		if(temp=='$'){
			for(int i=0; i<4; i++){
				response += fgetc(wifi_uart);
			}
			break;
		}
	}
	return response;
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

