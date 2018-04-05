#ifndef UI_HPP
#define UI_HPP

#include <functional>
#include <string>
#include <vector>
#include <exception>
#include <stdexcept>
#include <utility>
#include "SimpleGraphics.hpp"
#include "GeometricTypes.hpp"

namespace UI {
    using rgba_t = SimpleGraphics::rgba_t;
    using FontType = SimpleGraphicsFonts::FontType;

/* ------------------------------------------------------------------
 * An abstract class which represents anything that can be drawn
 */
class Drawable {
public:

    /* 
     * Draw the object on the screen
     */
    virtual void draw() = 0;

    /*
     * Undraw the object efficiently, so we don't need to clear the entire screen
     * Only implement this on classes where it makes sense to undraw
     */
    virtual void undraw() { throw std::logic_error("Undraw not implemented"); }

    Drawable(SimpleGraphics &graphics):
        m_graphics(graphics),
        m_is_showing(false)
    {}

    virtual ~Drawable(){};

protected:
    // Subclasses need access to these to draw themselves
    SimpleGraphics &m_graphics;
    bool m_is_showing;
};

/* ------------------------------------------------------------------
 * An abstract class which represents something that can be touched
 */
class Touchable {
public:
    /* Callback called everytime the object is touched
     * The object itself is provided along with the exact point of touch
     */
    typedef std::function<void(Point p)> TouchCB;

    /* Check if object is touched
     * 
     * If it is touched, the callback set with onTouch is 
     * run before returning
     */
    virtual bool touch(Point p) = 0;

    /* 
     * Set the function to be called when touched
     *
     * May not make sense for some classes
     */
    virtual void onTouch(TouchCB callback) { throw std::logic_error("onTouch not implemented"); };

    Touchable() {}

    virtual ~Touchable(){}

protected:
};

/* ------------------------------------------------------------------
 * Simple rectangle class
 */
class Rectangle : public Drawable {
public:
    virtual void draw();
    virtual void undraw();

    Rectangle(SimpleGraphics &graphics, Point p1, Point p2, rgba_t color);

    virtual ~Rectangle(){};

protected:
    Point m_p1, m_p2;
    rgba_t m_color;
};

/* ------------------------------------------------------------------
 * Simple button class
 */
class Button : public Rectangle, public Touchable {
public:
    virtual void draw();
    virtual void undraw();
    virtual bool touch(Point p);
    virtual void onTouch(TouchCB callback);

    Button(SimpleGraphics &graphics,
            Point p1, Point p2, std::string text, rgba_t text_color,
            rgba_t background_color, FontType f);

private:
    std::string m_text;
    rgba_t m_text_color;
    TouchCB m_cb;
    FontType m_font;
};


/* ------------------------------------------------------------------
 * Simple slider class
 */
class Slider : public Rectangle, public Touchable {
public:
    virtual void draw();
    virtual void undraw();
    virtual bool touch(Point p);

    int value() { return chosen_value; }

    Slider(SimpleGraphics &graphics, 
            Point p1, Point p2, rgba_t text_color,
            rgba_t background_color, int min, int max, FontType f);
    virtual void onTouch(TouchCB callback);

    int getMin() { return min; }
    int getMax() { return max; }
private:
    TouchCB m_cb;
    std::string m_text;
    rgba_t m_text_color;
    Point slider_p1, slider_p2;
    Point slider_bar_p1, slider_bar_p2;
    bool initial_state;
    int chosen_value;
    int min, max;
    FontType m_font;
};



/* ------------------------------------------------------------------
 * Simple dropdown menu class
 *
 * Note that we use protected inheritance to prevent others from
 * changing our onTouch function
 */
class DropdownMenu : public Drawable, public Touchable {
public:
    virtual void draw();
    virtual void undraw();
    virtual bool touch(Point p);

    // expand the dropdown menu when m_expander is pressed
    void expand();

    void close();

    /* 
     * Add a new button to the dropdown menu with text
     */
    void newItem(SimpleGraphics &graphics, std::string text, TouchCB callback);

    DropdownMenu(SimpleGraphics &graphics,
            Point p1, Point p2, rgba_t text_color,
            rgba_t background_color, FontType f);

    bool isOpen() { return m_is_open; }

private:

    // List of buttons in the menu
    std::vector<Button> m_buttons;

    // Points that define m_expander
    Point m_p1, m_p2;

    bool m_is_open;
    
    rgba_t m_background_colour;
    rgba_t m_text_colour;
    FontType m_font;
};

} // namespace UI

#endif
