#include "cursor.hpp"
#include "UI.hpp"
#include "SimpleGraphics.hpp"

namespace UI {

Cursor::Cursor(SimpleGraphics &graphics, SimpleGraphics::rgba_t color, int radius):
	Drawable(graphics),
	m_color(color),
    m_radius(radius),
    m_ColourPixels()
{}

void Cursor::draw(){
    // draw new Cursor
    // store new covered Pixcel Points
	draw_cursor(m_color, m_center, m_radius);
}

void Cursor::draw_cursor(SimpleGraphics::rgba_t color, Point p, int radius) {
    m_ColourPixels.clear();
	for (int i = -radius; i <= radius; i++) {
		for (int j = -radius; j <= radius; j++) {
			if (i == j || -i == j) {

				ColourPoint coveredPoint;
				coveredPoint.x = p.x+i;
				coveredPoint.y = p.y+j;
				coveredPoint.color = m_graphics.read_pixel(coveredPoint.x, coveredPoint.y);

				m_ColourPixels.push_back(coveredPoint);

				m_graphics.draw_pixel(color, p.x + i, p.y + j);
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
