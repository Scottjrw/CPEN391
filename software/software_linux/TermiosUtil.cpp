/*
 * TermiosUtil.cpp
 *
 *  Created on: 2018?3?18?
 *      Author: Jingrui
 */

#include "TermiosUtil.hpp"
#include <unistd.h>
#include <string>

namespace TermiosUtil{

	int SetSpeed(int fd, Baudrate rate){
		struct termios attribs;

		if(tcgetattr(fd, &attribs) < 0){
			perror("stdin");
		}


		if(rate==B115K){
			cfsetispeed(&attribs, B115200);
			cfsetospeed(&attribs, B115200);
		}else if(rate==B9K){
			cfsetispeed(&attribs, B9600);
			cfsetospeed(&attribs, B9600);
		}else if(rate==B38K){
			cfsetispeed(&attribs, B38400);
			cfsetospeed(&attribs, B38400);
		}else{
			printf("invalid baud rate");
		}

		cfmakeraw(&attribs);

		// apply setting
		if(tcsetattr(fd, TCSANOW, &attribs) < 0){
			perror("stdin");
			return EXIT_FAILURE;
		}

		/* data transmision should happen here */
		return EXIT_SUCCESS;
	}

}


