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
#include <TermiosUtil.hpp>


void Wifi::LedOn(){
	char command[] = "led_on()\r\n";
	if(write(wifi_uart, command, strlen(command))<0){
		printf("write failed\n");
	}
}

void Wifi::LedOff(){
	char command[] = "led_off()\r\n";
	if(write(wifi_uart, command, strlen(command))<0)
		printf("write failed\n");
}

void Wifi::LightOn(){
	char command[] = "light_on()\r\n";
	if(write(wifi_uart, command, strlen(command))<0)
		printf("write failed\n");
}

void Wifi::LightOff(){
	char command[] = "light_off()\r\n";
	if(write(wifi_uart, command, strlen(command))<0)
		printf("write failed\n");
}

void Wifi::SendUsername(std::string username, std::string password){
	std::string s= "send_username(\""+username+"\",\""+password+"\")\r\n";
	const char * command = s.c_str();
	if(write(wifi_uart, command, strlen(command))<0)
		printf("write failed\n");
}

void Wifi::SendPicture(std::string picture){

	std::string s1= "send_test()\r\n";
	// std::string s1= "send_picture()\r\n";
	const char * send = s1.c_str();

	int send_count = 0;
	for(int i=0; i<80*60*6; i+=160){
		std::string subpicture = picture.substr(i, 160);
		std::string s= "send_picture_part(\""+subpicture+"\")\r\n";
		const char * command = s.c_str();
		if(write(wifi_uart, command, strlen(command))<0)
			printf("write failed\n");
		usleep(50000);
		send_count++;
		if(send_count==10){
			if(write(wifi_uart, send, strlen(send))<0)
				printf("write failed\n");
			usleep(50000);
			send_count = 0;
		}
	}

	if(send_count>0){
		if(write(wifi_uart, send, strlen(send))<0)
			printf("write failed\n");
		usleep(50000);
		send_count = 0;
	}
}

std::string Wifi::ReadResponse(){
	//not sure working correctly
	std::string response;
	FILE * F = fdopen(wifi_uart, "r+");

	while(1){
		char temp = fgetc(F);
		printf("%c",temp);//for debug only
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

	if (wifi_uart < 0) {
		printf("Failed to open UART device\n");
		while (1)
			;
	}
	TermiosUtil::SetSpeed(wifi_uart, B115K);

}

