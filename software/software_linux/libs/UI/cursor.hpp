#ifndef CURSOR_HPP
#define CURSOR_HPP
/*
 * cursor.hpp
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

namespace UI {

class Cursor : public Drawable {
public:
    typedef std::array<std::array<SimpleGraphics::rgba_t, 50>, 50> cursor_bitmap;

    virtual void draw();
    virtual void undraw();

    void draw_cursor(Point p);

    Cursor(SimpleGraphics &graphics, cursor_bitmap bitmap);

    void update(Point center);

private:
    typedef struct {
        unsigned x;
        unsigned y;
        SimpleGraphics::rgba_t color;
    } ColourPoint;

    Point m_center;
    std::vector<ColourPoint> m_ColourPixels;
    
    cursor_bitmap m_bitmap;

};

};

#endif
