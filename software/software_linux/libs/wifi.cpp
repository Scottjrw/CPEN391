/*
 * wifi.c
 *
 *  Created on: Feb 6, 2018
 *      Author: Jingrui
 */

#include <string.h>
#include "wifi.hpp"
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


void Wifi::LedOn(){
	char command[] = "led_on()\r\n";
	write(wifi_uart, command, strlen(command));
}

void Wifi::LedOff(){
	char command[] = "led_off()\r\n";
	write(wifi_uart, command, strlen(command));
}

void Wifi::LightOn(){
	char command[] = "light_on()\r\n";
	write(wifi_uart, command, strlen(command));
}

void Wifi::LightOff(){
	char command[] = "light_off()\r\n";
	write(wifi_uart, command, strlen(command));
}

void Wifi::SendUsername(std::string username, std::string password){
	std::string s= "send_username(\""+username+"\",\""+password+"\")\r\n";
	const char * command = s.c_str();
	write(wifi_uart, command, strlen(command));
}

void Wifi::SendPicture(std::string picture){
	std::string s= "send_picture(\""+picture+"\")\r\n";
	const char * command = s.c_str();
	int status = write(wifi_uart, command, strlen(command));
}

std::string Wifi::ReadResponse(){
	//not sure working correctly
	std::string response;
	FILE * F = fdopen(wifi_uart, "r+");

	while(1){
		char temp = fgetc(F);
		if(temp=='$'){
			for(int i=0; i<4; i++){
				response += fgetc(F);
			}
			break;
		}
	}
	fclose(F);
	return response;
}

int Wifi::GetFd(){
	int i = wifi_uart;
	return i;
}

Wifi::Wifi(const char name[]) {
	wifi_uart = open(name, O_RDWR);

	if (wifi_uart == NULL) {
		printf("Failed to open UART device\n");
		while (1)
			;
	}

}

