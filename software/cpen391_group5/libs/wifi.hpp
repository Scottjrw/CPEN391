/*
 * wifi.h
 *
 *  Created on: Feb 6, 2018
 *      Author: Jingrui
 */

#ifndef WIFI_H_
#define WIFI_H_

#include <stdio.h>
#include <system.h>
#include <string>

class Wifi {
	public:
		Wifi(const char name[]);

		//void SendCommand(char command[]);
		void LedOn();
		void LedOff();
		void LightOn();
		void LightOff();
		void SendUsername(std::string username, std::string password);
		void SendPicture(std::string picture);
		std::string ReadResponse();

	private:
		FILE *wifi_uart;

};


#endif /* WIFI_H_ */

