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

class Wifi {
	public:
		Wifi(const char name[]);

		//void SendCommand(char command[]);
		void LedOn();
		void LedOff();
		void LightOn();
		void LightOff();

	private:
		FILE *wifi_uart;

};


#endif /* WIFI_H_ */

