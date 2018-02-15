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

namespace Wifi {

    constexpr const char * LED_ON = "led_on()\r\n";
    constexpr const char * LED_OFF = "led_off()\r\n";
    constexpr const char * LIGHT_ON = "light_on()\r\n";
    constexpr const char * LIGHT_OFF = "light_off()\r\n";

    void SendCommand(char command[], FILE *f);

    FILE* Init(char name[]);

};


#endif /* WIFI_H_ */

