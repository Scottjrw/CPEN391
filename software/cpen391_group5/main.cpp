#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "system.h"
#include "sys/alt_alarm.h"
#include "main.hpp"
#include "SimpleGraphics.hpp"
#include "vision_updated.hpp"
#include <iostream>
#include "video.h"
#include "screen.hpp"
#include "cursor.hpp"


int main(int argc, const char * argv[]) {

	if (initialize(VIDEO_UART_NAME)) {
		printf("File Opened Successfully\n");
	}
	else {
		printf("File Failed to Open\n");
	}

	if (imageSettings(0x00, 0x80, 0x80, 0x00, 0x7F)) {
		printf("changed colors\n");
	}
	else {
		printf("changed colors failed\n");
	}

	if (mirror_mode_on()) {
		printf("mirror successful\n");
	}
	else {
		printf("mirror failed\n");
	}






	SimpleGraphics graphics(reinterpret_cast<SimpleGraphics::rgba_t *>(DRAW_BUFFER_BASE),
			SG_MAX_WIDTH, SG_MAX_HEIGHT);

	TouchControl touch(TOUCHSCREEN_UART_NAME, TOUCHSCREEN_UART_IRQ, TOUCHSCREEN_UART_IRQ_INTERRUPT_CONTROLLER_ID,
			SG_MAX_WIDTH, SG_MAX_HEIGHT, true);

	Screen screen(graphics, touch);
	Button button(graphics, touch, {60, 0}, {160, 20}, "Change Config",
			SimpleGraphics::rgba(255, 255, 255, 255),
			SimpleGraphics::rgba(0, 0, 255, 100));

	Slider slider(graphics, touch, {60, 20}, {160, 40},
				SimpleGraphics::rgba(255, 255, 255, 255),
				SimpleGraphics::rgba(0, 255, 0, 100), 0, 50);


	DropdownMenu menu(graphics, touch, DropdownMenu::TOP, {0, 0}, {60, 20}, "Menu",
			SimpleGraphics::rgba(255, 255, 255, 255),
			SimpleGraphics::rgba(255, 255, 0, 100));

	menu.newItem(graphics, touch, "Option 1", SimpleGraphics::rgba(255, 255, 255, 255),
			SimpleGraphics::rgba(0, 0, 255, 100),
			[] (Touchable *, Point p) {
			std::cout << "Dropdown menu button got touched" << std::endl;

			});
	menu.newItem(graphics, touch, "Option 2", SimpleGraphics::rgba(255, 255, 255, 255),
				SimpleGraphics::rgba(0, 0, 255, 100),
				[] (Touchable *, Point p) {
				std::cout << "Dropdown menu button got touched" << std::endl;

				});
	menu.newItem(graphics, touch, "Option 3", SimpleGraphics::rgba(255, 255, 255, 255),
				SimpleGraphics::rgba(0, 0, 255, 100),
				[] (Touchable *, Point p) {
				std::cout << "Dropdown menu button got touched" << std::endl;

				});


	button.onTouch([] (Touchable *, Point p) {
			std::cout << "Button Got Touched" << std::endl;

			});

	slider.onTouch([] (Touchable *, Point p) {
				std::cout << "Slider Got Touched" << std::endl;

				});

	screen.addDrawable(&button);
	screen.addTouchable(&button);
	screen.addDrawable(&menu);
	screen.addTouchable(&menu);
	screen.addDrawable(&slider);
	screen.addTouchable(&slider);


	screen.draw();

	screen.enable_touch();

	touch.touch_enable();

	std::cout << "Running Graphics" << std::endl;




	std::cout << "Starting" << std::endl;

	Gesture_Recognizer::GestureCB cb_up = []{
		std::cout << "CB: UP" << std::endl;
	};

	Gesture_Recognizer::GestureCB cb_down = []{
		std::cout << "CB: DOWN" << std::endl;
	};

	Gesture_Recognizer::GestureCB cb_left = []{
		std::cout << "CB: LEFT" << std::endl;
	};

	Gesture_Recognizer::GestureCB cb_right = []{
		std::cout << "CB: RIGHT" << std::endl;
	};


	Gesture_Recognizer GR;

	GR.change_gesture_map(Direction(UP), cb_up);
	GR.change_gesture_map(Direction(DOWN), cb_down);
	GR.change_gesture_map(Direction(LEFT), cb_left);
	GR.change_gesture_map(Direction(RIGHT), cb_right);

	while(1) {
		GR.find_dots();
		UI::Point temp;
		temp.x = GR.Red_Point.x/2;
		temp.y = GR.Red_Point.y/2;
		screen.cursor.update(temp);
	}






//    struct point prev, cur;
//
//    while(1) {
//        //unsigned start = alt_nticks();
//        find_dots(&cur);
//        //printf("Algorithm took %lu ms\n", 1000 * (alt_nticks() - start) / alt_ticks_per_second());
//        gesture_detect(&prev, &cur);
//        memcpy(&prev, &cur, sizeof(struct point));
//    }
    
    return 0;
}
