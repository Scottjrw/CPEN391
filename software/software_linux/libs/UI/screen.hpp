#ifndef SCREEN_HPP
#define SCREEN_HPP
/*
 * screen.hpp
 *
 *  Created on: Feb 11, 2018
 *      Author: Daniel
 */

#include <functional>
#include <string>
#include <vector>
#include <exception>
#include <stdexcept>
#include <utility>
#include "UI.hpp"
#include "cursor.hpp"
#include "event_loop.hpp"
#include "touch.hpp"
#include "WandControl.hpp"

namespace UI {

class Screen : public Event_Loop, public Drawable, public Touchable {
public:
	/*
	 * draw all elements part of the screen
	 */
	virtual void draw();

    virtual void undraw();

    virtual bool touch(Point p);

	/*
	 * add an element to the screen
	 */
	void addDrawable(Drawable* element);

	void addTouchable(Touchable* element);

	Screen(SimpleGraphics &graphics, TouchControl &touch, 
			WandControl &wc);

    ~Screen();

private:
    TouchControl &m_touch;
	WandControl &m_wandControl;
	std::vector<Drawable*> m_drawables;
	std::vector<Touchable*> m_touchables;
	Cursor m_cursor;
    Cursor m_red_dot_cursor;

    static constexpr unsigned RED_DOT_POLL_MS = 50;

};

};

#endif /* SCREEN_HPP_ */
