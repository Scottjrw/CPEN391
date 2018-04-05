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
    static constexpr unsigned size = 32;
    typedef std::array<std::array<SimpleGraphics::rgba_t, size>, size> cursor_bitmap;
    using rgba_t = SimpleGraphics::rgba_t;

    virtual void draw();
    virtual void undraw();

    Cursor(SimpleGraphics &graphics, rgba_t bitmap[size][size]);

    void update(Point center);

private:
    Point m_center;
    
    cursor_bitmap m_saved;
    cursor_bitmap m_bitmap;

};

};

#endif
