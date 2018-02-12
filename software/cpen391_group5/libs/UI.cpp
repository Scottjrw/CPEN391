#include "UI.hpp"
#include "SimpleGraphics.hpp"

#define     CHAR_HEIGHT 7
#define     MENU_ITEM_WIDTH     30
#define     MENU_ITEM_HEIGHT    15
#define     MENU_ITEM_SPACE     5
using namespace UI;

Rectangle::Rectangle(SimpleGraphics &graphics, Point p1, Point p2, SimpleGraphics::rgba_t color):
    Drawable(graphics),
    m_p1(p1),
    m_p2(p2),
    m_color(color)
{
    m_is_Touchable = 0;
}

void Rectangle::draw() {

    // show a filled rectangle on screen 
    m_graphics.draw_rect(m_color, m_p1.x, m_p1.y, m_p2.x, m_p2.y);
}

void Rectangle::undraw() {

    // shows nothing on screen
    SimpleGraphics::rgba_t opaque_Color= m_graphics.rgba(0,0,0,0);
    m_graphics.draw_rect(opaque_Color, m_p1.x, m_p1.y, m_p2.x, m_p2.y);
}



Circle::Circle(SimpleGraphics &graphics, Point center, unsigned radius, SimpleGraphics::rgba_t color):
    Drawable(graphics),
    m_center(center),
    m_radius(radius),
    m_color(color)
{}

void Circle::draw() {

}

void Circle::undraw() {

}



Button::Button(SimpleGraphics &graphics, TouchControl &touch,
        Point p1, Point p2, std::string text, SimpleGraphics::rgba_t text_color,
        SimpleGraphics::rgba_t background_color):
	Rectangle(graphics, p1, p2, background_color),
    Touchable(touch),
    m_text(text),
    m_text_color(text_color)
{
    m_is_Touchable = 1;
}


void Button::draw(){
	Rectangle::draw();
    // display the name of the button

    unsigned strStartXPos = (((m_p2.x - m_p1.x) - m_text.length())/2) + m_p1.x;
    unsigned strStartYPos = (((m_p2.y - m_p1.y) - CHAR_HEIGHT)/2) +  m_p1.y;

    m_graphics.draw_string(m_text_color, strStartXPos, strStartYPos, m_text);

}

void Button::undraw(){
	Rectangle::undraw();
}

bool Button::touch(Point P){

    if(P.x > m_p1.x && P.x < m_p2.x && P.y > m_p1.y && P.y < m_p2.y)
        return true;
    else
        return false;
}

void Button::onTouch(TouchCB callback){

}


DropdownMenu::DropdownMenu(SimpleGraphics &graphics, TouchControl &touch,
        Expand direction, Point p1, Point p2, std::string text, SimpleGraphics::rgba_t text_color,
        SimpleGraphics::rgba_t background_color):
    Drawable(graphics),
    Touchable(touch),
    m_expandDir(direction),
    m_expander(graphics, touch, p1, p2, text, text_color, background_color),
    m_buttons(),
    m_p1(p1),
    m_p2(p2)
{
	auto cb = [this] (Touchable *, Point) {
		this->expand();
	};
	m_expander.onTouch(cb);
}

void DropdownMenu::draw(){
    m_expander.draw();

}

void DropdownMenu::undraw(){
    m_expander.draw();

}

bool DropdownMenu::touch(Point p){
    return m_expander.touch(p);
}

void DropdownMenu::expand(){

    //unsigned menu_width = m_p2.x - m_p1.x;
    //unsigned menu_height = m_buttons.size() * (MENU_ITEM_HEIGHT + MENU_ITEM_SPACE);

    // for now we just consider on direction: downward dropdown menu
    switch(m_expandDir){
        case TOP:

        case BOTTOM:

            for(int i = 0; i < (int)m_buttons.size(); i++){
                m_buttons[i].draw();
            }
    }
}

void DropdownMenu::close(){

    for(int i = 0; i < (int)m_buttons.size(); i++){
        m_buttons[i].undraw();
    }
}

void DropdownMenu::newItem(SimpleGraphics &graphics, TouchControl &touch, std::string text, SimpleGraphics::rgba_t text_color,
        SimpleGraphics::rgba_t background_color, TouchCB callback){
	int size = m_buttons.size();
	Point new_p1;
	new_p1.x = m_p1.x;
	new_p1.y = m_p1.y + ((size+1)*MENU_ITEM_HEIGHT);
	Point new_p2;
	new_p2.x = m_p2.x;
	new_p2.y = m_p2.y + ((size+1)*MENU_ITEM_HEIGHT);

	Button button = Button(graphics, touch, new_p1, new_p2, text, text_color, background_color);
	m_buttons.push_back(button);
}
