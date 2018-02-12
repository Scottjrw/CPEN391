/*
 * screen.hpp
 *
 *  Created on: Feb 11, 2018
 *      Author: Daniel
 */

#include <functional>
#include <string>
#include <vector>
#include <exception>
#include <stdexcept>
#include <utility>
#include "UI.hpp"

using namespace UI;

class Screen : public Drawable, public Touchable {
public:

	/*
	 * draw all elements part of the screen
	 */
	void draw();

	/*
	 * clear all the elements part of the screen
	 */
	void clear();


	void enable_touch();
	/*
	 * add an element to the screen
	 */
	void addElement(Drawable* element);

	/*
	 * add an element to the screen
	 */
	void addElement(Touchable* element);

	Screen(SimpleGraphics &graphics, TouchControl &touch);




private:
	std::vector<Drawable*> drawables;
	std::vector<Touchable*> touchables;



};


#endif /* SCREEN_HPP_ */
