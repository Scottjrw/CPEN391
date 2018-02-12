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
{}

void Rectangle::draw() {

    // show a filled rectangle on screen 
    m_graphics.draw_rect(m_color, m_p1.x, m_p1.y, m_p2.x, m_p2.y);
}

void Rectangle::undraw() {

    // shows nothing on screen
    SimpleGraphics::rgba_t opaque_Color= m_graphics.rgba(0,0,0,0);
    m_graphics.draw_rect(opaque_Color, m_p1.x, m_p1.y, m_p2.x, m_p2.y);
}




Button::Button(SimpleGraphics &graphics, TouchControl &touch,
        Point p1, Point p2, std::string text, SimpleGraphics::rgba_t text_color,
        SimpleGraphics::rgba_t background_color):
	Rectangle(graphics, p1, p2, background_color),
    Touchable(touch),
    m_text(text),
    m_text_color(text_color),
    m_cb(nullptr)
{ }


void Button::draw(){
	Rectangle::draw();
    // display the name of the button
	unsigned remaining_space_x;
	unsigned text_width = 5*m_text.length();

	if ((m_p2.x - m_p1.x) > text_width) {
		remaining_space_x = (m_p2.x - m_p1.x) - text_width;
	} else
		remaining_space_x = 4;

    unsigned strStartXPos = (remaining_space_x/2) + m_p1.x;

	unsigned remaining_space_y;

	if ((m_p2.y - m_p1.y) > CHAR_HEIGHT) {
		remaining_space_y = (m_p2.y - m_p1.y) - CHAR_HEIGHT;
	} else
		remaining_space_y = 4;

    unsigned strStartYPos = (remaining_space_y/2) +  m_p1.y;

    m_graphics.draw_string(m_text_color, strStartXPos, strStartYPos, m_text);

}

void Button::undraw(){
	Rectangle::undraw();

	unsigned remaining_space_x;
	unsigned text_width = 5*m_text.length();

	if ((m_p2.x - m_p1.x) > text_width) {
		remaining_space_x = (m_p2.x - m_p1.x) - text_width;
	} else
		remaining_space_x = 4;

	unsigned strStartXPos = (remaining_space_x/2) + m_p1.x;

	unsigned remaining_space_y;

	if ((m_p2.y - m_p1.y) > CHAR_HEIGHT) {
		remaining_space_y = (m_p2.y - m_p1.y) - CHAR_HEIGHT;
	} else
		remaining_space_y = 4;

	unsigned strStartYPos = (remaining_space_y/2) +  m_p1.y;

	m_graphics.draw_string(m_graphics.rgba(0,0,0,0), strStartXPos, strStartYPos, m_text);
}

bool Button::touch(Point P){

    if(P.x > m_p1.x && P.x < m_p2.x && P.y > m_p1.y && P.y < m_p2.y) {
    	if (m_cb != nullptr) m_cb(this, P);
        return true;
    } else
        return false;
}

void Button::onTouch(TouchCB callback){
	m_cb = callback;
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
    m_p2(p2),
    m_is_open(false)
{
	auto cb = [this] (Touchable *, Point) {
		if (m_is_open) this->close();
		else this->expand();
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
	for (Button temp : m_buttons) {
		if (temp.touch(p)) return true;
	}
	if (m_expander.touch(p)) return true;
	return false;
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
    m_is_open = true;
}

void DropdownMenu::close(){

    for(int i = 0; i < (int)m_buttons.size(); i++){
        m_buttons[i].undraw();
    }
    m_is_open = false;
}

void DropdownMenu::newItem(SimpleGraphics &graphics, TouchControl &touch, std::string text, SimpleGraphics::rgba_t text_color,
        SimpleGraphics::rgba_t background_color, TouchCB callback){
	int size = m_buttons.size();
	Point new_p1;
	int height = m_p2.y - m_p1.y;
	new_p1.x = m_p1.x;
	new_p1.y = m_p1.y + ((size+1)*height);
	Point new_p2;
	new_p2.x = m_p2.x;
	new_p2.y = m_p2.y + ((size+1)*height);

	Button button = Button(graphics, touch, new_p1, new_p2, text, text_color, background_color);
	button.onTouch(callback);
	m_buttons.push_back(button);
}
