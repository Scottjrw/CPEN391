/*
 * TermiosUtil.hpp
 *
 *  Created on: 2018?3?18?
 *      Author: Jingrui
 */

#ifndef TERMIOSUTIL_HPP_
#define TERMIOSUTIL_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

enum Baudrate
{
	B115K, B9K, B38K
};

namespace TermiosUtil{
	int SetSpeed(int fd, Baudrate rate);
}




#endif /* TERMIOSUTIL_HPP_ */
