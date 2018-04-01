/*
 * wifi.h
 *
 *  Created on: Feb 6, 2018
 *      Author: Jingrui
 */

#ifndef WIFI_H_
#define WIFI_H_

#include <stdio.h>
#include <string>
#include <termios.h>
#include <map>

class Wifi {
	public:
		Wifi(const char name[]);

		void LedOn();
		void LedOff();
		void LightOn();
		void LightOff();
		void SendUsername(std::string username, std::string password);
		void SendPicture(std::string picture);
		std::string ReadResponse();
		std::map<std::string, std::string> GetGestureMapping();
		void SendTriggeredGesture(std::string gesture);

	private:
	 	int wifi_uart;
	 	static constexpr int subpicture_length = 160;
	 	static constexpr int pixel_length = 6;

};


#endif /* WIFI_H_ */

