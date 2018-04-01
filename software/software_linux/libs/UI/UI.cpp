#include "UI.hpp"
#include "SimpleGraphics.hpp"
#include <stdio.h>



namespace UI {

namespace Settings {
    constexpr unsigned CHAR_HEIGHT = 14;
    constexpr unsigned MENU_ITEM_WIDTH = 30;
    constexpr unsigned MENU_ITEM_HEIGHT = 15;
    constexpr unsigned MENU_ITEM_SPACE = 5;
};

Rectangle::Rectangle(SimpleGraphics &graphics, Point p1, Point p2, rgba_t color):
    Drawable(graphics),
    m_p1(p1),
    m_p2(p2),
    m_color(color)
{}

void Rectangle::draw() {

    // show a filled rectangle on screen 
    m_graphics.draw_rect(m_color, m_p1.x, m_p1.y, m_p2.x, m_p2.y);
    //usleep(500);
}

void Rectangle::undraw() {

    // shows nothing on screen
    rgba_t opaque_Color= rgba(0,0,0,0);
    m_graphics.draw_rect(opaque_Color, m_p1.x, m_p1.y, m_p2.x, m_p2.y);
}




Button::Button(SimpleGraphics &graphics,
        Point p1, Point p2, std::string text, rgba_t text_color,
        rgba_t background_color):
	Rectangle(graphics, p1, p2, background_color),
    Touchable(),
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

	if ((m_p2.y - m_p1.y) > Settings::CHAR_HEIGHT) {
		remaining_space_y = (m_p2.y - m_p1.y) - Settings::CHAR_HEIGHT;
	} else
		remaining_space_y = 4;

    unsigned strStartYPos = (remaining_space_y/2) +  m_p1.y;

    m_graphics.draw_string(m_text_color, strStartXPos, strStartYPos, m_text);

    m_is_showing = true;

}

void Button::undraw(){
	Rectangle::undraw();

	unsigned remaining_space_x;
	unsigned text_width = 10*m_text.length();

	if ((m_p2.x - m_p1.x) > text_width) {
		remaining_space_x = (m_p2.x - m_p1.x) - text_width;
	} else
		remaining_space_x = 4;

	unsigned strStartXPos = (remaining_space_x/2) + m_p1.x;

	unsigned remaining_space_y;

	if ((m_p2.y - m_p1.y) > Settings::CHAR_HEIGHT) {
		remaining_space_y = (m_p2.y - m_p1.y) - Settings::CHAR_HEIGHT;
	} else
		remaining_space_y = 4;

	unsigned strStartYPos = (remaining_space_y/2) +  m_p1.y;

	m_graphics.draw_string(rgba(0,0,0,0), strStartXPos, strStartYPos, m_text);

	m_is_showing = false;
}

bool Button::touch(Point P){

	if (m_is_showing){
		if(P.x > m_p1.x && P.x < m_p2.x && P.y > m_p1.y && P.y < m_p2.y) {
			if (m_cb != nullptr) m_cb(P);
			return true;
		} else
			return false;
	}
	else return false;
}

void Button::onTouch(TouchCB callback){
	m_cb = callback;
}

Slider::Slider(SimpleGraphics &graphics,
        Point p1, Point p2, rgba_t text_color,
        rgba_t background_color, int min, int max):
	Rectangle(graphics, p1, p2, background_color),
    Touchable(),
    m_text("00"),
    m_text_color(text_color),
    slider_p1(),
    slider_p2(),
    slider_bar_p1(),
    slider_bar_p2(),
    m_background_color(background_color),
    initial_state(),
    chosen_value(min),
    min(min),
    max(max)
{
	initial_state = true;
}


void Slider::draw(){
	printf("%d\n", initial_state);
	Rectangle::draw();
    // display the value of the slider

	unsigned remaining_space_x;
	unsigned text_width = 5*m_text.length();

	if ((m_p2.x - m_p1.x) > text_width) {
		remaining_space_x = (m_p2.x - m_p1.x) - text_width;
	} else
		remaining_space_x = 4;

    unsigned strStartXPos = m_p2.x - 8;

	unsigned remaining_space_y;

	if ((m_p2.y - m_p1.y) > Settings::CHAR_HEIGHT) {
		remaining_space_y = (m_p2.y - m_p1.y) - Settings::CHAR_HEIGHT;
	} else
		remaining_space_y = 4;

    unsigned strStartYPos = (remaining_space_y/2) +  m_p1.y;

    slider_bar_p1.x = m_p1.x + 2;
    slider_bar_p1.y = strStartYPos + 3;

    slider_bar_p2.x = m_p1.x + remaining_space_x - 4;
    slider_bar_p2.y = strStartYPos + 5;

    Rectangle slider_bar(m_graphics, slider_bar_p1, slider_bar_p2, rgba(255, 255, 255, 255));
    slider_bar.draw();

    if (initial_state){
		m_graphics.draw_string(m_text_color, strStartXPos - 4, strStartYPos, m_text);

		slider_p1.x = slider_bar_p1.x;
		slider_p1.y = m_p1.y + 2;
		slider_p2.x = slider_bar_p1.x + 2;
		slider_p2.y = m_p2.y - 2;

		Rectangle slider(m_graphics, slider_p1, slider_p2, rgba(0, 0, 0, 255));
		slider.draw();
    }

    m_is_showing = true;

}

void Slider::undraw(){
	Rectangle::undraw();

	unsigned remaining_space_x;
	unsigned text_width = 5*m_text.length();

	if ((m_p2.x - m_p1.x) > text_width) {
		remaining_space_x = (m_p2.x - m_p1.x) - text_width;
	} else
		remaining_space_x = 4;

	unsigned strStartXPos = (remaining_space_x/2) + m_p1.x;

	unsigned remaining_space_y;

	if ((m_p2.y - m_p1.y) > Settings::CHAR_HEIGHT) {
		remaining_space_y = (m_p2.y - m_p1.y) - Settings::CHAR_HEIGHT;
	} else
		remaining_space_y = 4;

	unsigned strStartYPos = (remaining_space_y/2) +  m_p1.y;

	m_graphics.draw_string(rgba(0,0,0,0), strStartXPos, strStartYPos, m_text);

	m_is_showing = false;
}

bool Slider::touch(Point P){
	if (m_is_showing){
		if(P.x > slider_bar_p1.x && P.x < slider_bar_p2.x && P.y > m_p1.y && P.y < m_p2.y) {
			initial_state = false;
			unsigned strStartXPos = m_p2.x - 12;

			unsigned remaining_space_y;

			if ((m_p2.y - m_p1.y) > Settings::CHAR_HEIGHT) {
				remaining_space_y = (m_p2.y - m_p1.y) - Settings::CHAR_HEIGHT;
			} else
				remaining_space_y = 4;

			unsigned strStartYPos = (remaining_space_y/2) +  m_p1.y;

			draw();

			slider_p1.x = P.x - 1;
			slider_p1.y = m_p1.y + 2;
			slider_p2.x = P.x + 1;
			slider_p2.y = m_p2.y - 2;

			float slider_width = slider_bar_p2.x - slider_bar_p1.x;
			float relative_touch = P.x - slider_bar_p1.x;
			float ratio = relative_touch/slider_width;
			int range = max-min;

			chosen_value = ratio*range;

			m_text[0] = (int)(chosen_value)/10 + '0';
			m_text[1] = (int)(chosen_value)%10 + '0';

			m_graphics.draw_string(m_text_color, strStartXPos, strStartYPos, m_text);

			Rectangle slider(m_graphics, slider_p1, slider_p2, rgba(0, 0, 0, 255));
			slider.draw();
            return true;
		}
	}

	return false;
}


DropdownMenu::DropdownMenu(SimpleGraphics &graphics,
        Point p1, Point p2, std::string text, rgba_t text_color,
        rgba_t background_color):
    Drawable(graphics),
    Touchable(),
    m_expander(graphics, p1, p2, text, text_color, background_color),
    m_buttons(),
    m_p1(p1),
    m_p2(p2),
    m_is_open(false)
{
	auto cb = [this] (auto) -> bool {
		if (m_is_open) this->close();
		else this->expand();
	};
	m_expander.onTouch(cb);
}

void DropdownMenu::draw(){
    m_expander.draw();
    m_is_showing = true;

}

void DropdownMenu::undraw(){
	DropdownMenu::close();
    m_expander.undraw();
    m_is_showing = false;

}

bool DropdownMenu::touch(Point p){
	if (m_is_showing){
		if (m_is_open){
			for (Button temp : m_buttons) {
				if (temp.touch(p)) return true;
			}
		}
		if (m_expander.touch(p)) return true;
				return false;
	}
	else return false;
}

void DropdownMenu::expand(){

    // for now we just consider on direction: downward dropdown menu
    for(int i = 0; i < (int)m_buttons.size(); i++){
        m_buttons[i].draw();
    }
    m_is_open = true;
}

void DropdownMenu::close(){

    for(int i = 0; i < (int)m_buttons.size(); i++){
        m_buttons[i].undraw();
    }
    m_is_open = false;
}

void DropdownMenu::newItem(SimpleGraphics &graphics, std::string text, rgba_t text_color,
        rgba_t background_color, TouchCB callback){
	int size = m_buttons.size();
	Point new_p1;
	int height = m_p2.y - m_p1.y;
	new_p1.x = m_p1.x;
	new_p1.y = m_p1.y + ((size+1)*height);
	Point new_p2;
	new_p2.x = m_p2.x;
	new_p2.y = m_p2.y + ((size+1)*height);

	Button button = Button(graphics, new_p1, new_p2, text, text_color, background_color);
	button.onTouch(callback);
	m_buttons.push_back(button);
}

};
