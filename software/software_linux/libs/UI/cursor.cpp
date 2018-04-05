#include "cursor.hpp"
#include "UI.hpp"
#include "SimpleGraphics.hpp"

namespace UI {

Cursor::Cursor(SimpleGraphics &graphics, cursor_bitmap bitmap):
	Drawable(graphics),
    m_center(),
    m_ColourPixels(),
    m_bitmap(bitmap)
{}

void Cursor::draw(){
    // draw new Cursor
    // store new covered Pixcel Points
	draw_cursor(m_center);
}

void Cursor::draw_cursor(Point p) {
    m_ColourPixels.clear();
    for (int i = 0; i < 50; i++) {
        for (int j = 0; j < 50; j++) {
            if (m_bitmap[i][j] != 0) {
                ColourPoint coveredPoint;
                coveredPoint.x = p.x+i;
                coveredPoint.y = p.y+j;
                coveredPoint.color = m_graphics.read_pixel(coveredPoint.x, coveredPoint.y);
                
                m_ColourPixels.push_back(coveredPoint);
                
                m_graphics.draw_pixel(m_bitmap[i][j], p.x + i, p.y + j);
            }
        }
    }
}

void Cursor::undraw(){

    // restore previously covered points
    while(!m_ColourPixels.empty()){

        // get previously covered points
    	ColourPoint prev_ColourPixel = m_ColourPixels.back();
        m_graphics.draw_pixel(prev_ColourPixel.color, prev_ColourPixel.x, prev_ColourPixel.y);
        m_ColourPixels.pop_back();
    }
}

void Cursor::update(Point center){
    draw();
    m_center = center;
    undraw();
}

}
