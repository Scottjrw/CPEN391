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

#define LED_ON "led_on()\r\n"
#define LED_OFF "led_off()\r\n"

void WIFISendCommand(char command[], FILE *f);

FILE* WIFIInit(char name[]);


#endif /* WIFI_H_ */

