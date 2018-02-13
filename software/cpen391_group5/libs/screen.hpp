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

using namespace UI;

class Screen : public Drawable, public Touchable {
public:

	/*
	 * draw all elements part of the screen
	 */
	virtual void draw();

	/*
	 * clear all the elements part of the screen
	 */
	void clear();

    virtual bool touch(Point p) { throw std::logic_error("Cannot Touch Screen"); }

	void enable_touch();
	/*
	 * add an element to the screen
	 */
	void addDrawable(Drawable* element);

	void addTouchable(Touchable* element);

	Screen(SimpleGraphics &graphics, TouchControl &touch);

private:
	std::vector<Drawable*> drawables;
	std::vector<Touchable*> touchables;
	Cursor cursor;

};


#endif /* SCREEN_HPP_ */
