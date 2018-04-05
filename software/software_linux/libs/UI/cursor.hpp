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
    typedef std::array<std::array<SimpleGraphics::rgba_t, 32>, 32> cursor_bitmap;

    virtual void draw();
    virtual void undraw();

    Cursor(SimpleGraphics &graphics, cursor_bitmap bitmap);

    void update(Point center);

private:
    Point m_center;
    
    cursor_bitmap m_bitmap;
    cursor_bitmap m_saved;

};

};

#endif
