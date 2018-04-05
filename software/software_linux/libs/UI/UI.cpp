#include "UI.hpp"
#include "SimpleGraphics.hpp"
#include <stdio.h>



namespace UI {


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
        rgba_t background_color, FontType f):
	Rectangle(graphics, p1, p2, background_color),
    Touchable(),
    m_text(text),
    m_text_color(text_color),
    m_cb(nullptr),
    m_font(f)
{ }


void Button::draw(){
	Rectangle::draw();
    
    m_graphics.draw_string_centered(m_text_color, (m_p2.x - m_p1.x)/2 + m_p1.x, ((m_p2.y - m_p1.y))/2 + m_p1.y, m_text, m_font);

    m_is_showing = true;

}

void Button::undraw(){
	Rectangle::undraw();

    m_graphics.draw_string_centered(rgba(0,0,0,0), (m_p2.x - m_p1.x)/2 + m_p1.x, ((m_p2.y - m_p1.y))/2 + m_p1.y, m_text, m_font);

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
        rgba_t background_color, int min, int max, FontType f):
	Rectangle(graphics, p1, p2, background_color),
    Touchable(),
    m_cb(nullptr),
    m_text("00"),
    m_text_color(text_color),
    slider_p1(),
    slider_p2(),
    slider_bar_p1(),
    slider_bar_p2(),
    initial_state(),
    chosen_value(min),
    min(min),
    max(max),
    m_font(f)
{
	initial_state = true;
}


void Slider::draw(){
	printf("%d\n", initial_state);
	Rectangle::draw();
    
    slider_bar_p1.x = m_p1.x + 10;
    slider_bar_p1.y = (m_p2.y - m_p1.y)/3 + m_p1.y;
    
    slider_bar_p2.x = m_p2.x - 48;
    slider_bar_p2.y = 2*(m_p2.y - m_p1.y)/3 + m_p1.y;

    Rectangle slider_bar(m_graphics, slider_bar_p1, slider_bar_p2, rgba(150, 150, 150, 255));
    slider_bar.draw();

    if (initial_state){
		m_graphics.draw_string_centered(m_text_color, (m_p2.x - slider_bar_p2.x)/2 + slider_bar_p2.x, (m_p2.y - m_p1.y)/2 + m_p1.y, m_text, m_font);

		slider_p1.x = slider_bar_p1.x;
		slider_p1.y = m_p1.y + 5;
		slider_p2.x = slider_bar_p1.x + 20;
		slider_p2.y = m_p2.y - 5;

		Rectangle slider(m_graphics, slider_p1, slider_p2, rgba(0, 0, 0, 255));
		slider.draw();
    }

    m_is_showing = true;

}

void Slider::undraw(){
	Rectangle::undraw();

    m_graphics.draw_string_centered(rgba(0,0,0,0), (m_p2.x - slider_bar_p2.x)/2 + slider_bar_p2.x, (m_p2.y - m_p1.y)/2 + m_p1.y, m_text, m_font);

	m_is_showing = false;
}

bool Slider::touch(Point P){
	if (m_is_showing){
		if(P.x > slider_bar_p1.x && P.x < slider_bar_p2.x && P.y > m_p1.y && P.y < m_p2.y) {
			initial_state = false;
            
			draw();

			slider_p1.x = P.x - 10;
			slider_p1.y = m_p1.y + 5;
			slider_p2.x = P.x + 10;
			slider_p2.y = m_p2.y - 5;

			float slider_width = slider_bar_p2.x - slider_bar_p1.x;
			float relative_touch = P.x - slider_bar_p1.x;
			float ratio = relative_touch/slider_width;
			int range = max-min;

			chosen_value = ratio*range;

            
			m_graphics.draw_string_centered(m_text_color, (m_p2.x - slider_bar_p2.x)/2 + slider_bar_p2.x, (m_p2.y - m_p1.y)/2 + m_p1.y, m_text, m_font);

			Rectangle slider(m_graphics, slider_p1, slider_p2, rgba(0, 0, 0, 255));
			slider.draw();


		}

        if(P.x > m_p1.x && P.x < m_p2.x && P.y > m_p1.y && P.y < m_p2.y) {
            if (m_cb != nullptr) m_cb(P);
                return true;
        } else
            return false;
	}

	return false;
}

void Slider::onTouch(TouchCB callback){
    m_cb = callback;
}


DropdownMenu::DropdownMenu(SimpleGraphics &graphics,
        Point p1, Point p2, rgba_t text_color,
        rgba_t background_color, FontType f):
    Drawable(graphics),
    Touchable(),
    m_buttons(),
    m_p1(p1),
    m_p2(p2),
    m_is_open(false),
    m_background_colour(background_color),
    m_text_colour(text_color),
    m_font(f)
{
}

void DropdownMenu::draw(){
    m_is_showing = true;

}

void DropdownMenu::undraw(){
	DropdownMenu::close();
    m_is_showing = false;

}

bool DropdownMenu::touch(Point p){
	if (m_is_showing){
		if (m_is_open){
			for (Button temp : m_buttons) {
				if (temp.touch(p)) return true;
			}
		}
        return false;
	}
    else {
        return false;
    }
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

void DropdownMenu::newItem(SimpleGraphics &graphics, std::string text, TouchCB callback){
	int size = m_buttons.size();
	Point new_p1;
	int height = m_p2.y - m_p1.y;
	new_p1.x = m_p1.x;
	new_p1.y = m_p1.y + ((size)*height);
	Point new_p2;
	new_p2.x = m_p2.x;
	new_p2.y = m_p2.y + ((size)*height);

	Button button = Button(graphics, new_p1, new_p2, text, m_text_colour, m_background_colour, m_font);
	button.onTouch(callback);
	m_buttons.push_back(button);
}

};
