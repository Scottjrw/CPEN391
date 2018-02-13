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
#include "vision.hpp"



using namespace UI;

class Cursor : public Drawable{

public:
    Cursor(SimpleGraphics &graphics, Point center, SimpleGraphics::rgba_t color, int radius);

    virtual void draw();
    virtual void undraw();

    void update(unsigned x, unsigned y);


private:
    Point m_center;
    SimpleGraphics::rgba_t m_color;
    int m_radius;
    vector<struct PointPixel> m_pointPixels;

}  