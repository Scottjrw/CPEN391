#include "cursor.hpp"
#include "UI.hpp"
#include "SimpleGraphics.hpp"

namespace UI {

Cursor::Cursor(SimpleGraphics &graphics, cursor_bitmap bitmap):
	Drawable(graphics),
    m_center(),
    m_bitmap(bitmap)
{}

void Cursor::draw(){
    for (int y = 0; y < m_saved.size(); y++) {
        for (int x = 0; x < m_saved.size(); x++) {
            m_saved[x][y] = m_graphics.read_pixel(m_center.x + x, m_center.y + y);
        }

    }
}

void Cursor::undraw(){
    for (int y = 0; y < m_saved.size(); y++) {
        for (int x = 0; x < m_saved.size(); x++) {
            m_graphics.draw_pixel(m_saved[x][y], m_center.x + x, m_center.y + y);
        }
    }
}

void Cursor::update(Point center){
    m_center = center;
}

}
