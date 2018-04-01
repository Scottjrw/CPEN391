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

    virtual void draw();
    virtual void undraw();

    void draw_cursor(SimpleGraphics::rgba_t color, Point p, int radius);

    Cursor(SimpleGraphics &graphics, SimpleGraphics::rgba_t color, int radius);

    void update(Point center);

private:
    typedef struct {
        unsigned x;
        unsigned y;
        SimpleGraphics::rgba_t color;
    } ColourPoint;

    Point m_center;
    SimpleGraphics::rgba_t m_color;
    int m_radius;
    std::vector<ColourPoint> m_ColourPixels;

};

};

#endif
