#include "UI.hpp"

using namespace UI;

Rectangle::Rectangle(SimpleGraphics &graphics, Point p1, Point p2):
    Drawable(graphics),
    m_p1(p1),
    m_p2(p2)
{}



Circle::Circle(SimpleGraphics &graphics, Point center, unsigned radius):
    Drawable(graphics),
    m_center(center),
    m_radius(radius)
{}


Button::Button(SimpleGraphics &graphics, TouchControl &touch,
        Point p1, Point p2, std::string text):
    Rectangle(graphics, p1, p2),
    Touchable(touch),
    m_text(text)
{}


DropdownMenu::DropdownMenu(SimpleGraphics &graphics, TouchControl &touch,
        Point p1, Point p2, std::string text):
    Drawable(graphics),
    Touchable(touch),
    m_expander(graphics, touch, p1, p2, text),
    m_buttons()
{}

