#ifndef UI_HPP
#define UI_HPP

#include <functional>
#include <string>
#include <vector>
#include <exception>
#include <stdexcept>
#include <utility>
#include "SimpleGraphics.hpp"
#include "touch.hpp"


namespace UI {

/* Represents a x,y coordinate
 * first is x, second is y
 */
typedef struct {unsigned x; unsigned y;} Point;

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

    /*
     * Move the object in the X or Y by amount pixels, sleeping sleep usec after each pixel
     * May be useful to reduce redraws, but primarily for animation
     */
    virtual void translateX(int amount, unsigned sleepUs=0) { throw std::logic_error("Cannot be moved"); }
    virtual void translateY(int amount, unsigned sleepUs=0) { throw std::logic_error("Cannot be moved"); }

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
    typedef std::function<void(Touchable *, Point)> TouchCB;

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

    Touchable(TouchControl &touch):
        m_touch(touch)
    {}

    virtual ~Touchable(){};

protected:
    TouchControl &m_touch;
};

/* ------------------------------------------------------------------
 * Simple rectangle class
 */
class Rectangle : public Drawable {
public:
    virtual void draw();
    virtual void undraw();

    Rectangle(SimpleGraphics &graphics, Point p1, Point p2, SimpleGraphics::rgba_t color);

    virtual ~Rectangle(){};

protected:
    Point m_p1, m_p2;
    SimpleGraphics::rgba_t m_color;
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

    Button(SimpleGraphics &graphics, TouchControl &touch,
            Point p1, Point p2, std::string text, SimpleGraphics::rgba_t text_color,
            SimpleGraphics::rgba_t background_color);

private:
    std::string m_text;
    SimpleGraphics::rgba_t m_text_color;
    TouchCB m_cb;
};


/* ------------------------------------------------------------------
 * Simple slider class
 */
class Slider : public Rectangle, public Touchable {
public:
    virtual void draw();
    virtual void undraw();
    virtual bool touch(Point p);
    virtual void onTouch(TouchCB callback);

    int value() { return chosen_value; }

    Slider(SimpleGraphics &graphics, TouchControl &touch,
            Point p1, Point p2, SimpleGraphics::rgba_t text_color,
            SimpleGraphics::rgba_t background_color, int min, int max);

private:
    std::string m_text;
    SimpleGraphics::rgba_t m_text_color;
    TouchCB m_cb;
    Point slider_p1, slider_p2;
    Point slider_bar_p1, slider_bar_p2;
    SimpleGraphics::rgba_t m_background_color;
    bool initial_state;
    int chosen_value;
    int min, max;
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
    void newItem(SimpleGraphics &graphics, TouchControl &touch, std::string text, SimpleGraphics::rgba_t text_color,
            SimpleGraphics::rgba_t background_color, TouchCB callback);

    enum Expand { TOP, BOTTOM };
    DropdownMenu(SimpleGraphics &graphics, TouchControl &touch,
            Expand direction, Point p1, Point p2, std::string text, SimpleGraphics::rgba_t text_color,
            SimpleGraphics::rgba_t background_color);

private:
    Expand m_expandDir;
    // Button which shows the menu
    Button m_expander;
    // List of buttons in the menu
    std::vector<Button> m_buttons;

    // Points that define m_expander
    Point m_p1, m_p2;

    bool m_is_open;
};

} // namespace UI

#endif