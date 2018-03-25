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
//#include "PixelCluster.hpp"

namespace UI {

class Screen : public Drawable, public Touchable {
public:
	/*
	 * draw all elements part of the screen
	 */
	virtual void draw();

    virtual void undraw();

    virtual bool touch(Point p) { throw std::logic_error("Cannot Touch Screen"); }

	void enable_touch();
	/*
	 * add an element to the screen
	 */
	void addDrawable(Drawable* element);

	void addTouchable(Touchable* element);

	int run(void);

	void exit(int ret_code);

    //void addPixelCluster(PixelCluster *pixel);

	Screen(SimpleGraphics &graphics, TouchControl &touch);

private:
	bool m_exited;
	std::vector<Drawable*> m_drawables;
	std::vector<Touchable*> m_touchables;
    //PixelCluster *m_cluster;
	int m_next_screen;
	Cursor m_cursor;
    Cursor m_red_dot_cursor;

    static constexpr unsigned RED_DOT_POLL_MS = 50;

    void touchCB(TouchControl *, unsigned x, unsigned y);

};

};

#endif /* SCREEN_HPP_ */
