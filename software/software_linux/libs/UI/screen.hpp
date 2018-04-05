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

class Screen : public Event_Loop, public Drawable, public WandControl {
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

    void remDrawable(Drawable *element);

    void remTouchable(Touchable *element);

	Screen(SimpleGraphics &graphics, TouchControl &touch, 
            GeometricRecognizer &geo, Wand &wand, NIOS_Processor &nios,
            Cursor &gesture, Cursor &typing, Cursor &mouse);

    ~Screen();

protected:
    virtual void updateCursor(Point p);
    virtual void cursorModeChange(Wand::Modes old_mode);
private:
    TouchControl &m_touch;
	std::vector<Drawable*> m_drawables;
	std::vector<Touchable*> m_touchables;
    Cursor &m_gestureCursor;
    Cursor &m_typingCursor;
    Cursor &m_mouseCursor;

    void update_cursor_pos(Point p);
    bool redraw_cursor();

    void draw_cursor(Wand::Modes mode);
    void draw_cursor() { draw_cursor(m_mode); }
    void undraw_cursor(Wand::Modes mode);
    void undraw_cursor() { undraw_cursor(m_mode); }
};

};

#endif /* SCREEN_HPP_ */
