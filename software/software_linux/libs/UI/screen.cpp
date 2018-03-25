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
        unsigned width, unsigned height):
	Drawable(graphics),
	Touchable(touch),
    m_width(width),
    m_height(height),
	m_exited(false),
	m_drawables(),
	m_touchables(),
	m_next_screen(),
	m_cursor(m_graphics, SimpleGraphics::rgba(0, 255, 0, 255), 4),
    m_red_dot_cursor(m_graphics, SimpleGraphics::rgba(0, 0, 255, 255), 4)
{
}

void Screen::draw() {
	for (unsigned i = 0; i < m_drawables.size(); i++) {
		m_drawables[i]->draw();
	}
}

void Screen::undraw() {
	for (unsigned i = 0; i < m_drawables.size(); i++) {
		m_drawables[i]->undraw();
	}
}

void Screen::enable_touch() {
    using namespace std::placeholders;

	m_touch.setTouchCB(std::bind(&Screen::touchCB, this, _1, _2, _3));
}

void Screen::touchCB(TouchControl *screenptr, unsigned x, unsigned y) {

    Point p = {x, y};

    // undraw previous cursor
    // restore the image that previous cursor covers

    // draw new cursor at Point p

    m_cursor.undraw();

    for (auto touch : m_touchables) {
        if (touch->touch(p)) break;
    }

    m_cursor.update(p);
    m_cursor.draw();
}


void Screen::addDrawable(Drawable* element) {
	m_drawables.push_back(element);
}

void Screen::addTouchable(Touchable* element) {
	m_touchables.push_back(element);
}

// void Screen::addPixelCluster(PixelCluster *pixel) {
//     m_cluster = pixel;
// }


int Screen::run(void) {
    using namespace std::chrono;

    auto last_poll_ticks = high_resolution_clock::now();
	while(m_exited == false){
		m_touch.trypoll();

        auto now = high_resolution_clock::now();
        milliseconds ms = duration_cast<milliseconds>(now - last_poll_ticks);

        //if (ms.count() > RED_DOT_POLL_MS) {
        //    unsigned x, y, count;
        //    m_cluster->poll(x, y, count);

        //    m_red_dot_cursor.undraw();

        //    m_red_dot_cursor.update({x, y});

        //    m_red_dot_cursor.draw();

        //    last_poll_ticks = now;
        //}

	}

	return m_next_screen;
}

void Screen::exit(int ret_code) {
	m_exited = true;
	m_next_screen = ret_code;
}

};


