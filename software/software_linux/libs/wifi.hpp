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
#include <unordered_map>

class Wifi {
	public:
		Wifi(const char name[]);

		void LedOn();
		void LedOff();
		void LightOn();
		void LightOff();
		void SendUsername(std::string username, std::string password);
		void SendPicture(std::string picture);
		bool ReadResponse();
		void LoadGestureMapping();
		std::unordered_map<std::string, std::string> GetGestureMapping();
		void SendTriggeredGesture(std::string gesture);
		void SendGestureMapping(std::unordered_map<std::string, std::string>);

	private:
	 	int wifi_uart;
	 	FILE * F;
	 	std::unordered_map<std::string, std::string> gesture_map;
	 	static constexpr int subpicture_length = 160;
	 	static constexpr int pixel_length = 6;

	 	// flush previous output; need time for wifi to reconnect
		void Reset();

};


#endif /* WIFI_H_ */

