#include "cursor.hpp"
#include "UI.hpp"
#include "SimpleGraphics.hpp"

namespace UI {

Cursor::Cursor(SimpleGraphics &graphics, rgba_t bitmap[size][size]):
	Drawable(graphics),
    m_center(),
    m_bitmap()
{
    for (unsigned y = 0; y < size; y++) {
        for (unsigned x = 0; x < size; x++) {
            m_bitmap[x][y] = bitmap[y][x];
        }
    }
}


void Cursor::draw(){
    for (unsigned y = 0; y < size; y++) {
        for (unsigned x = 0; x < size; x++) {
            if ((m_bitmap[x][y] & 0xFF000000) == 0xFF000000) {
                m_saved[x][y] = m_graphics.read_pixel(m_center.x + x, m_center.y + y);
                m_graphics.draw_pixel(m_bitmap[x][y], m_center.x + x, m_center.y + y);
            }
        }

    }
}

void Cursor::undraw(){
    for (unsigned y = 0; y < size; y++) {
        for (unsigned x = 0; x < size; x++) {
            if ((m_bitmap[x][y] & 0xFF000000) == 0xFF000000)
                m_graphics.draw_pixel(m_saved[x][y], m_center.x + x, m_center.y + y);
        }
    }
}

void Cursor::update(Point center){
    m_center = center;
}

}
