#include "cursor.hpp"
#include "UI.hpp"
#include "SimpleGraphics.hpp"

Cursor::Cursor(SimpleGraphics &graphics, Point center, SimpleGraphics::rgba_t color, int radius):
    m_color(color),
    m_radius(radius),
    m_center(center)
{}

void Cursor::draw(){
    // draw new Cursor
    // store new covered Pixcel Points
    SimpleGraphics::draw_x(m_color, m_center.x, m_center.y, m_radius, m_pointPixels);
}

void Cursor::undraw(){

    // restore previously covered points
    while(!m_pointPixels.empty()){

        // get previously covered points
        PointPixel prev_PointPixel = m_pointPixels.back();
        draw_pixel(prev_PointPixel.color, prev_PointPixel.x, prev_PointPixel.y);
        m_pointPixels.pop_back();
    }
}

