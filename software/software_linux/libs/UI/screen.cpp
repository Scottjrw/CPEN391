/*
 * screen.cpp
 *
 *  Created on: Feb 11, 2018
 *      Author: Daniel
 */

#include "screen.hpp"
#include <stdio.h>
#include <chrono>

namespace UI {

Screen::Screen(SimpleGraphics &graphics, TouchControl &touch,
    WandControl &wc):
    Event_Loop(),
	Drawable(graphics),
	Touchable(),
    m_touch(touch),
    m_wandControl(wc),
	m_drawables(),
	m_touchables(),
	m_cursor(m_graphics, rgba(0, 255, 0, 255), 4),
    m_red_dot_cursor(m_graphics, rgba(0, 0, 255, 255), 4)
{
    add(&m_touch, &TouchControl::trypoll);
    m_touch.setTouchable(*this);
    m_wandControl.setCursorClickCB(std::bind(&Screen::touch, this, std::placeholders::_1), 
        std::bind(&Screen::touch, this, std::placeholders::_1));
}

Screen::~Screen() {
    m_touch.clearTouchable();
}

void Screen::draw() {
    m_graphics.clear();

	for (unsigned i = 0; i < m_drawables.size(); i++) {
		m_drawables[i]->draw();
	}
}

void Screen::undraw() {
	for (unsigned i = 0; i < m_drawables.size(); i++) {
		m_drawables[i]->undraw();
	}

    m_graphics.clear();
}

bool Screen::touch(Point p) {

    // undraw previous cursor
    // restore the image that previous cursor covers

    // draw new cursor at Point p

    m_cursor.undraw();

    for (auto touch : m_touchables) {
        if (touch->touch(p)) break;
    }

    m_cursor.update(p);
    m_cursor.draw();

    return true;
}


void Screen::addDrawable(Drawable* element) {
	m_drawables.push_back(element);
}

void Screen::addTouchable(Touchable* element) {
	m_touchables.push_back(element);
}

};


