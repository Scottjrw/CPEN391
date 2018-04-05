#include "screen.hpp"
#include <stdio.h>
#include <chrono>
#include <algorithm>

namespace UI {

Screen::Screen(SimpleGraphics &graphics, TouchControl &touch, 
        Wand &wand, NIOS_Processor &nios,
        Cursor &gesture, Cursor &typing, Cursor &mouse):
    Event_Loop(),
	Drawable(graphics),
    WandControl(wand, nios),
    m_touch(touch),
	m_drawables(),
	m_touchables(),
    m_gestureCursor(gesture),
    m_typingCursor(typing),
    m_mouseCursor(mouse)
{
    add(&m_touch, &TouchControl::trypoll);
    m_touch.setTouchable(*this);
    add_timer(std::chrono::milliseconds(1000/12), this, &Screen::redraw_cursor);
}

Screen::~Screen() {
    m_touch.clearTouchable();
    m_graphics.clear();
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

    undraw_cursor();

    for (auto touch : m_touchables) {
        if (touch->touch(p)) break;
    }

    draw_cursor();

    return true;
}


void Screen::addDrawable(Drawable* element) {
	m_drawables.push_back(element);
}

void Screen::addTouchable(Touchable* element) {
	m_touchables.push_back(element);
}

void Screen::remDrawable(Drawable *element) {
    std::remove(m_drawables.begin(), m_drawables.end(), element);
}

void Screen::remTouchable(Touchable* *element) {
    std::remove(m_touchables.begin(), m_touchables.end(), element);
}

void Screen::updateCursor(Point p) {
    m_mouseCursor.update(p);
    m_typingCursor.update(p);
    m_gestureCursor.update(p);
}

void Screen::cursorModeChange(Wand::Modes old_mode) {
    undraw_cursor(old_mode);
    draw_cursor();
}

bool Screen::redraw_cursor() {
    undraw_cursor();

    draw_cursor();
    return true;
}

void Screen::draw_cursor(Wand::Modes mode) {
    switch(mode) {
        case Wand::cursorMode:
            m_mouseCursor.draw();
            break;

        case Wand::gestureMode:
            m_gestureCursor.draw();
            break;

        case Wand::typingMode:
            m_typingCursor.draw();
            break;
    }
}

void Screen::undraw_cursor(Wand::Modes mode) {
    switch(mode) {
        case Wand::cursorMode:
            m_mouseCursor.undraw();
            break;

        case Wand::gestureMode:
            m_gestureCursor.undraw();
            break;

        case Wand::typingMode:
            m_typingCursor.undraw();
            break;
    }
}

}
