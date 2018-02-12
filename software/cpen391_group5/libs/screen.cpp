/*
 * screen.cpp
 *
 *  Created on: Feb 11, 2018
 *      Author: Daniel
 */


#include "screen.hpp"

using namespace UI;

Screen::Screen(SimpleGraphics &graphics, TouchControl &touch):
		Drawable(graphics),
		Touchable(touch),
		drawables(),
		touchables()
	{}

void Screen::draw() {
	for (int i = 0; i < (int)drawables.size(); i++) {
		drawables[i]->draw();
	}

}

void Screen::enable_touch() {
	m_touch.setTouchCB([](TouchControl *, unsigned x, unsigned y) {
		Point p = {x, y};
		for (auto touch : touchables) {
			if (touch->touch(p)) break;
		}
	});
}

void Screen::clear() {
	for (int i = 0; i < (int)drawables.size(); i++) {
		drawables[i]->undraw();
	}
}


void Screen::addElement(Drawable* element) {
	drawables.push_back(element);
}

void Screen::addElement(Touchable* element) {
	touchables.push_back(element);
}
