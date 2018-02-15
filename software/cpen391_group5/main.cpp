#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "system.h"
#include "sys/alt_alarm.h"
#include "SimpleGraphics.hpp"
#include "vision_updated.hpp"
#include <iostream>
#include "video.h"
#include "screen.hpp"
#include "cursor.hpp"
#include "UI.hpp"
#include "io.h"
#include <stdio.h>

constexpr unsigned SG_MAX_WIDTH = 160;
constexpr unsigned SG_MAX_HEIGHT = 120;

int brightness = 0x00;
int contrast = 0x80;
int saturation = 0x40;

void home_screen(SimpleGraphics &graphics, TouchControl &touch, Screen &screen);
void image_settings_screen(SimpleGraphics &graphics, TouchControl &touch, Screen &screen);
void gestures_settings_screen(SimpleGraphics &graphics, TouchControl &touch, Screen &screen);


int main(int argc, const char * argv[]) {


	/*
	 * Only initialize this camera stuff the first time
	 */
	if (Video::initialize(VIDEO_UART_NAME)) {
		printf("File Opened Successfully\n");
	}
	else {
		printf("File Failed to Open\n");
	}

	if (Video::imageSettings(brightness, contrast, 0x80, 0x00, saturation)) {
		printf("changed colors\n");
	}
	else {
		printf("changed colors failed\n");
	}

	if (Video::mirror_mode_on()) {
		printf("mirror successful\n");
	}
	else {
		printf("mirror failed\n");
	}



	SimpleGraphics graphics(reinterpret_cast<SimpleGraphics::rgba_t *>(DRAW_BUFFER_BASE),
			SG_MAX_WIDTH, SG_MAX_HEIGHT);

	TouchControl touch(TOUCHSCREEN_UART_NAME, TOUCHSCREEN_UART_IRQ, TOUCHSCREEN_UART_IRQ_INTERRUPT_CONTROLLER_ID,
			SG_MAX_WIDTH, SG_MAX_HEIGHT, true);

	Current_Screen s(HOME);

	while (1){
		Screen screen(graphics, touch);
		switch(s) {
		case HOME: 
        {
            Video::imageSettings(brightness, contrast, 0x80, 0x00, saturation);
			Button title(graphics, touch, {0, 0}, {100, 30}, "LIGHT CONTROLLER",
					SimpleGraphics::rgba(0, 0, 0, 255),
					SimpleGraphics::rgba(255, 255, 153, 175));

			Button team_name(graphics, touch, {100, 0}, {160, 10}, "by team 5",
						SimpleGraphics::rgba(255, 255, 255, 255),
						SimpleGraphics::rgba(0, 0, 0, 255));

			DropdownMenu menu(graphics, touch, DropdownMenu::TOP, {100, 10}, {160, 30}, "Menu",
					SimpleGraphics::rgba(255, 255, 255, 255),
					SimpleGraphics::rgba(89, 89, 89, 150));

			menu.newItem(graphics, touch, "Change IMG", SimpleGraphics::rgba(255, 255, 255, 255),
					SimpleGraphics::rgba(166, 166, 166, 100),
					[&screen] (Touchable *, Point p) {
					std::cout << "Change Image Settings" << std::endl;
					screen.clear();
                    Video::imageSettings(0x7F, contrast, 0x0A, 0x00, 0x00);
					screen.exit(IMAGE_SETTINGS);

					});
			menu.newItem(graphics, touch, "Gestures", SimpleGraphics::rgba(255, 255, 255, 255),
						SimpleGraphics::rgba(166, 166, 166, 100),
						[&screen] (Touchable *, Point p) {
						std::cout << "Change Gesture Mappings" << std::endl;
						screen.clear();
                        Video::imageSettings(0x7F, contrast, 0x0A, 0x00, 0x00);
						screen.exit(Current_Screen(GESTURE_SETTINGS));

						});


			title.onTouch([] (Touchable *, Point p) {
					std::cout << "LIGHT CONTROLLER" << std::endl;

					});

			screen.addDrawable(&title);
			screen.addTouchable(&title);
			screen.addDrawable(&team_name);
			screen.addDrawable(&menu);
			screen.addTouchable(&menu);

			screen.draw();
			screen.enable_touch();
			touch.touch_enable();
			s = screen.run();
			break;
		}

		case IMAGE_SETTINGS: {
			int temp_brightness;
			int temp_contrast;
			int temp_saturation;

			Slider brightness_slider(graphics, touch, {30, 30}, {130, 45},
						SimpleGraphics::rgba(0, 0, 0, 255),
						SimpleGraphics::rgba(163, 163, 163, 255), 0, 99);
			brightness_slider.onTouch([&temp_brightness, &brightness_slider] (Touchable *, Point p) {
						std::cout << "brightness changing" << std::endl;
						temp_brightness = (float)(brightness_slider.chosen_value)/(float)(brightness_slider.max - brightness_slider.min)*127;

						});

			Slider contrast_slider(graphics, touch, {30, 60}, {130, 75},
						SimpleGraphics::rgba(0, 0, 0, 255),
						SimpleGraphics::rgba(163, 163, 163, 255), 0, 99);
			contrast_slider.onTouch([&temp_contrast, &contrast_slider] (Touchable *, Point p) {
						std::cout << "contrast changing" << std::endl;
						temp_contrast = (float)(contrast_slider.chosen_value)/(float)(contrast_slider.max - contrast_slider.min)*255;

						});

			Slider saturation_slider(graphics, touch, {30, 90}, {130, 105},
						SimpleGraphics::rgba(0, 0, 0, 255),
						SimpleGraphics::rgba(163, 163, 163, 255), 0, 99);
			saturation_slider.onTouch([&temp_saturation, &saturation_slider] (Touchable *, Point p) {
						std::cout << "saturation changing" << std::endl;
						temp_saturation = (float)(saturation_slider.chosen_value)/(float)(saturation_slider.max - saturation_slider.min)*255;
						});

			Button back(graphics, touch, {0, 0}, {27, 120}, "exit",
						SimpleGraphics::rgba(255, 255, 255, 255),
						SimpleGraphics::rgba(100, 100, 100, 255));

			back.onTouch([&screen] (Touchable *, Point p) {
					std::cout << "BACK" << std::endl;
					screen.clear();
					screen.exit(Current_Screen(HOME));

					});

			Button save(graphics, touch, {133, 0}, {160, 120}, "save",
						SimpleGraphics::rgba(255, 140, 102, 255),
						SimpleGraphics::rgba(100, 100, 100, 255));

			save.onTouch([&temp_brightness, &temp_contrast, &temp_saturation, &screen] (Touchable *, Point p) {
					std::cout << "SAVE" << std::endl;
					brightness = temp_brightness;
					contrast = temp_contrast;
					saturation = temp_saturation;
					screen.clear();
					screen.exit(Current_Screen(HOME));

					});



		//	image_settings.addDrawable(&background);
			screen.addDrawable(&brightness_slider);
			screen.addTouchable(&brightness_slider);
			screen.addDrawable(&contrast_slider);
			screen.addTouchable(&contrast_slider);
			screen.addDrawable(&saturation_slider);
			screen.addTouchable(&saturation_slider);
			screen.addDrawable(&back);
			screen.addTouchable(&back);
			screen.addDrawable(&save);
			screen.addTouchable(&save);

			screen.draw();

			screen.enable_touch();

			graphics.draw_string(SimpleGraphics::rgba(0, 0, 0, 255), 43, 5, "IMAGE SETTINGS");

			graphics.draw_string(SimpleGraphics::rgba(0, 0, 0, 255), 55, 20, "BRIGHTNESS");
			graphics.draw_string(SimpleGraphics::rgba(0, 0, 0, 255), 60, 50, "CONTRAST");
			graphics.draw_string(SimpleGraphics::rgba(0, 0, 0, 255), 55, 80, "SATURATION");
			s = screen.run();
			break;
		}

		case GESTURE_SETTINGS: {
			Screen gesture_settings(graphics, touch);
			int temp_brightness;
			int temp_contrast;
			int temp_saturation;

			Button back(graphics, touch, {0, 0}, {27, 120}, "exit",
						SimpleGraphics::rgba(255, 255, 255, 255),
						SimpleGraphics::rgba(100, 100, 100, 255));

			back.onTouch([&screen] (Touchable *, Point p) {
					std::cout << "BACK" << std::endl;
					screen.clear();
					screen.exit(Current_Screen(HOME));

					});

			Button save(graphics, touch, {133, 0}, {160, 120}, "save",
						SimpleGraphics::rgba(255, 140, 102, 255),
						SimpleGraphics::rgba(100, 100, 100, 255));

			save.onTouch([&temp_brightness, &temp_contrast, &temp_saturation, &screen] (Touchable *, Point p) {
					std::cout << "SAVE" << std::endl;
					screen.clear();
					screen.exit(Current_Screen(HOME));

					});

			DropdownMenu actions(graphics, touch, DropdownMenu::TOP, {30, 20}, {78, 30}, "ACTIONS",
					SimpleGraphics::rgba(255, 255, 255, 255),
					SimpleGraphics::rgba(60, 60, 60, 255));

			actions.newItem(graphics, touch, "Light On", SimpleGraphics::rgba(255, 255, 255, 255),
					SimpleGraphics::rgba(70, 70, 70, 255),
					[] (Touchable *, Point p) {
					std::cout << "Turn On Lights" << std::endl;

					});
			actions.newItem(graphics, touch, "Light Off", SimpleGraphics::rgba(255, 255, 255, 255),
					SimpleGraphics::rgba(80, 80, 80, 255),
					[] (Touchable *, Point p) {
					std::cout << "Turn Off Lights" << std::endl;

					});
			actions.newItem(graphics, touch, "Increase", SimpleGraphics::rgba(255, 255, 255, 255),
					SimpleGraphics::rgba(90, 90, 90, 255),
					[] (Touchable *, Point p) {
					std::cout << "Increase Brightness" << std::endl;

					});
			actions.newItem(graphics, touch, "Decrease", SimpleGraphics::rgba(255, 255, 255, 255),
					SimpleGraphics::rgba(100, 100, 100, 255),
					[] (Touchable *, Point p) {
					std::cout << "Decrease Brightness" << std::endl;

					});

			DropdownMenu gestures(graphics, touch, DropdownMenu::TOP, {82, 20}, {130, 30}, "GESTURES",
					SimpleGraphics::rgba(255, 255, 255, 255),
					SimpleGraphics::rgba(60, 60, 60, 255));

			gestures.newItem(graphics, touch, "Up", SimpleGraphics::rgba(255, 255, 255, 255),
					SimpleGraphics::rgba(70, 70, 70, 255),
					[] (Touchable *, Point p) {
					std::cout << "Up" << std::endl;

					});
			gestures.newItem(graphics, touch, "Down", SimpleGraphics::rgba(255, 255, 255, 255),
					SimpleGraphics::rgba(80, 80, 80, 255),
					[] (Touchable *, Point p) {
					std::cout << "Down" << std::endl;

					});
			gestures.newItem(graphics, touch, "Left", SimpleGraphics::rgba(255, 255, 255, 255),
					SimpleGraphics::rgba(90, 90, 90, 255),
					[] (Touchable *, Point p) {
					std::cout << "Left" << std::endl;

					});
			gestures.newItem(graphics, touch, "Right", SimpleGraphics::rgba(255, 255, 255, 255),
					SimpleGraphics::rgba(100, 100, 100, 255),
					[] (Touchable *, Point p) {
					std::cout << "Right" << std::endl;

					});
			gestures.newItem(graphics, touch, "Up-Left", SimpleGraphics::rgba(255, 255, 255, 255),
					SimpleGraphics::rgba(110, 110, 110, 255),
					[] (Touchable *, Point p) {
					std::cout << "Up-Left" << std::endl;

					});
			gestures.newItem(graphics, touch, "Down-Left", SimpleGraphics::rgba(255, 255, 255, 255),
					SimpleGraphics::rgba(120, 120, 120, 255),
					[] (Touchable *, Point p) {
					std::cout << "Down-Left" << std::endl;

					});
			gestures.newItem(graphics, touch, "Up-Right", SimpleGraphics::rgba(255, 255, 255, 255),
					SimpleGraphics::rgba(130, 130, 130, 255),
					[] (Touchable *, Point p) {
					std::cout << "Up-Right" << std::endl;

					});
			gestures.newItem(graphics, touch, "Up-Left", SimpleGraphics::rgba(255, 255, 255, 255),
					SimpleGraphics::rgba(140, 140, 140, 255),
					[] (Touchable *, Point p) {
					std::cout << "Down-Right" << std::endl;

					});



		//	gesture_settings.addDrawable(&background);
			screen.addDrawable(&back);
			screen.addTouchable(&back);
			screen.addDrawable(&save);
			screen.addTouchable(&save);
			screen.addDrawable(&actions);
			screen.addTouchable(&actions);
			screen.addDrawable(&gestures);
			screen.addTouchable(&gestures);

			screen.draw();

			screen.enable_touch();

			graphics.draw_string(SimpleGraphics::rgba(0, 0, 0, 255), 40, 5, "GESTURE SETTINGS");
			s = screen.run();
			break;
		}
		}

	}


	/*
	 * Still have to implement the Dot Recognition
	 */


//	Cursor red_dot_cursor(graphics, SimpleGraphics::rgba(255, 0, 0, 255), 4);
//
//
//
//	std::cout << "Starting" << std::endl;
//
//	Gesture_Recognizer::GestureCB cb_up = []{
//		std::cout << "CB: UP" << std::endl;
//	};
//
//	Gesture_Recognizer::GestureCB cb_down = []{
//		std::cout << "CB: DOWN" << std::endl;
//	};
//
//	Gesture_Recognizer::GestureCB cb_left = []{
//		std::cout << "CB: LEFT" << std::endl;
//	};
//
//	Gesture_Recognizer::GestureCB cb_right = []{
//		std::cout << "CB: RIGHT" << std::endl;
//	};
//
//
//	Gesture_Recognizer GR;
//
//	GR.change_gesture_map(Direction(UP), cb_up);
//	GR.change_gesture_map(Direction(DOWN), cb_down);
//	GR.change_gesture_map(Direction(LEFT), cb_left);
//	GR.change_gesture_map(Direction(RIGHT), cb_right);
//
//	UI::Point temp;
//
//	while(1) {
//		GR.find_dots();
//		if ((temp.x == GR.Red_Point.x/2) && (temp.y == GR.Red_Point.y/2)){
//			red_dot_cursor.undraw();
//		}
//		else {
//			temp.x = GR.Red_Point.x/2;
//			temp.y = GR.Red_Point.y/2;
//			red_dot_cursor.update(temp);
//		}
//		touch.trypoll();
//	}
//
    return 0;
}
