/*
 * screen.cpp
 *
 *  Created on: Feb 11, 2018
 *      Author: Daniel
 */


#include "screen.hpp"
#include <stdio.h>

using namespace UI;

Screen::Screen(SimpleGraphics &graphics, TouchControl &touch):
	Drawable(graphics),
	Touchable(touch),
	cursor(m_graphics, SimpleGraphics::rgba(0, 255, 0, 255), 4),
	exited(false),
	drawables(),
	touchables(),
	Next_Screen()
{
}

void Screen::draw() {
	Rectangle clear_rect(m_graphics, {0, 0}, {160, 120}, SimpleGraphics::rgba(0, 0, 0, 0));
	clear_rect.draw();

	for (int i = 0; i < (int)drawables.size(); i++) {
		drawables[i]->draw();
	}
}

void Screen::enable_touch() {

	std::cout << "Enable Touch" << std::endl;
	m_touch.setTouchCB([this](TouchControl *, unsigned x, unsigned y) {
		Point p = {x, y};

		// undraw previous cursor
		// restore the image that previous cursor covers

		// draw new cursor at Point p

		cursor.undraw();

		for (auto touch : touchables) {
			if (touch->touch(p)) break;
		}
		cursor.update(p);
	});
}

void Screen::clear() {
	for (int i = 0; i < (int)drawables.size(); i++) {
		drawables[i]->undraw();
	}
}


void Screen::addDrawable(Drawable* element) {
	drawables.push_back(element);
}

void Screen::addTouchable(Touchable* element) {
	touchables.push_back(element);
}


Current_Screen Screen::run(void) {
	while(exited == false){
		m_touch.trypoll();
	}
	return Next_Screen;

}

void Screen::exit(Current_Screen New_Screen) {
	exited = true;
	Next_Screen = New_Screen;
}




