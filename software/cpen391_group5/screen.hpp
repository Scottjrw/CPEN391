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
	Screen(SimpleGraphics &graphics, TouchControl &touch);

	void draw();
	void clear();
	void addElement(Drawable* element);




private:
	std::vector<Drawable*> all_elements;
	std::vector<Touchable*> touchables;



};


#endif /* SCREEN_HPP_ */
