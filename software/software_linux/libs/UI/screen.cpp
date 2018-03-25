/*
 * screen.cpp
 *
 *  Created on: Feb 11, 2018
 *      Author: Daniel
 */

#include "screen.hpp"
#include <stdio.h>
#include <chrono>

using namespace UI;

Screen::Screen(SimpleGraphics &graphics, TouchControl &touch):
	Drawable(graphics),
	Touchable(touch),
	m_exited(false),
	m_drawables(),
	m_touchables(),
	m_next_screen(),
	cursor(m_graphics, SimpleGraphics::rgba(0, 255, 0, 255), 4),
    red_dot_cursor(m_graphics, SimpleGraphics::rgba(0, 0, 255, 255), 4)
{
}

void Screen::draw() {
	Rectangle clear_rect(m_graphics, {0, 0}, {160, 120}, SimpleGraphics::rgba(0, 0, 0, 0));
	clear_rect.draw();

	for (unsigned i = 0; i < m_drawables.size(); i++) {
		m_drawables[i]->draw();
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

		for (auto touch : m_touchables) {
			if (touch->touch(p)) break;
		}
		cursor.update(p);
        cursor.draw();
	});
}

void Screen::clear() {
	for (unsigned i = 0; i < m_drawables.size(); i++) {
		m_drawables[i]->undraw();
	}
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


Current_Screen Screen::run(void) {
    std::chrono::time_point<std::chrono::high_resolution_clock> last_poll_ticks;
	while(m_exited == false){
		m_touch.trypoll();

        std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> ms = (1000 * (now - last_poll_ticks)) ;   

        if (ms.count() > RED_DOT_POLL_MS) {
            unsigned x, y, count;
            //m_cluster->poll(x, y, count);

            red_dot_cursor.undraw();

            red_dot_cursor.update({x/2, y/2});

            red_dot_cursor.draw();

            last_poll_ticks = now;
        }

	}
	return m_next_screen;

}

void Screen::exit(Current_Screen next_screen) {
	m_exited = true;
	m_next_screen = next_screen;
}




