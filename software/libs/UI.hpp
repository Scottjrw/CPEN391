#ifndef UI_HPP
#define UI_HPP

#include <string>
#include <vector>
#include <functional>
#include <exception>
#include <stdexcept>
#include <utility>
#include "SimpleGraphics.hpp"

// TODO: Finish touch
class TouchControl;

namespace UI {

/* Represents a x,y coordinate
 * first is x, second is y
 */
typedef std::pair<unsigned, unsigned> Point;

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
        m_graphics(graphics)
    {}

protected:
    // Subclasses need access to these to draw themselves
    SimpleGraphics &m_graphics;
};

/* ------------------------------------------------------------------
 * An abstract class which represents something that can be touched
 */
class Touchable {
public:
    typedef std::function<void(Touchable &)> TouchCB;

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

    Rectangle(SimpleGraphics &graphics, Point p1, Point p2);

private:
    Point m_p1, m_p2;

};

/* ------------------------------------------------------------------
 * Simple circle class
 */
class Circle : public Drawable {
public:
    virtual void draw();
    virtual void undraw();

    Circle(SimpleGraphics &graphics, Point center, unsigned radius);

private:
    Point m_center;
    unsigned m_radius;

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
            Point p1, Point p2, std::string text);

private:
    std::string m_text;

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

    /* 
     * Add a new button to the dropdown menu with text
     */
    virtual void newItem(std::string text, TouchCB callback);

    DropdownMenu(SimpleGraphics &graphics, TouchControl &touch,
            Point p1, Point p2, std::string text);

private:
    // Button which shows the menu
    Button m_expander;
    // List of buttons in the menu
    std::vector<Button> m_buttons;
};

} // namespace UI

#endif
