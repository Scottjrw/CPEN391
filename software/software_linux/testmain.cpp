
#include <system_error>
#include <unistd.h>
#include <iostream>

#include "video.hpp"
#include "fifo_serial.hpp"
#include "NIOS_Processor.hpp"
#include "arm_system.h"
#include "GeometricRecognizer.hpp"
#include "GeometricRecognizerTypes.hpp"
#include "UI.hpp"
#include "touch.hpp"
#include "event_loop.hpp"
#include "screen.hpp"
#include "Close_Handler.hpp"
#include "NIOS_Processor_Init.hpp"
#include "wifi.hpp"
#include "TermiosUtil.hpp"

using namespace UI;
using namespace DollarRecognizer;
namespace SimpleGraphicsFonts;

enum Switch_Page_Commands
{
    to_Home = 0,
    to_GestureMap = 2,
    to_NewGesture = 3,
    to_Setting = 4,
    to_LoginPanel = 5
}

void addDropDownMenu(SimpleGraphics &sg, screen &sc, FontType menuFont){
    DropdownMenu homeMenu(sg, {40, 40}, {240, 100}, "Menu", rgba(0, 0, 0, 255), rgba(159, 159, 159, 255), menuFont);
    homeMenu.newItem(sg, "Home", rgba(80, 80, 80, 255), rgba(180, 180, 180, 255), [&sc](Point p) {
                sc.stop(0);
    });

    homeMenu.newItem(sg, "Gesture Map", rgba(80, 80, 80, 255), rgba(180, 180, 180, 255), [&sc](Point p) {
                sc.stop(2);
    });

    homeMenu.newItem(sg, "New Gesture", rgba(80, 80, 80, 255), rgba(180, 180, 180, 255), [&sc](Point p) {
                sc.stop(3);
    });

    homeMenu.newItem(sg, "Settings", rgba(80, 80, 80, 255), rgba(180, 180, 180, 255), [&sc](Point p) {
                sc.stop(4);
    });

    homeMenu.newItem(sg, "Log Out", rgba(80, 80, 80, 255), rgba(180, 180, 180, 255), [&sc](Point p) {
                sc.stop(5);
    });

    sc.addDrawable(homeMenu);
    sc.addTouchable(homeMenu);
}
void

showStartScreen(SimpleGraphics &sg, Video &video, TouchControl &tc)
{
}

/*
 * @Param:
 *      username:  callback from login() is login successfully
 *      mistery class reference: implement on Tuesday
 */
int showLoginPanel(SimpleGraphics &sg, Wifi &wifi, Wand &wand, GeometricRecognizer &gr, Video &video, NIOS_Processor &nios)
{

    // lp is a subclass of screen
    LoginPanel lp(sg, wifi, video, wc, {0, 0}, {640, 480}, Font16x27, Font10x14);

    return lp.run();
}

int showSetting(SimpleGraphics &sg, FontType buttonFont, FontType sliderFont, FontType menuFont, TouchControl &tc)
{
    screen sc(sg, tc, wc);

    addDropDownMenu(sg, sc, FontType menuFont);

    int temp_brightness;
    int temp_contrast;
    int temp_saturation;

    Slider brightness_slider(graphics, {30, 30}, {130, 45},
                             SimpleGraphics::rgba(0, 0, 0, 255),
                             SimpleGraphics::rgba(163, 163, 163, 255), 0, 100, sliderFont);
    brightness_slider.onTouch([&temp_brightness, &brightness_slider](Touchable *, Point p) {
        std::cout << "brightness changing" << std::endl;
        temp_brightness = (float)(brightness_slider.chosen_value) / (float)(brightness_slider.max - brightness_slider.min) * 127;

    });

    Slider contrast_slider(graphics, {30, 60}, {130, 75},
                           SimpleGraphics::rgba(0, 0, 0, 255),
                           SimpleGraphics::rgba(163, 163, 163, 255), 0, 100, sliderFont);
    contrast_slider.onTouch([&temp_contrast, &contrast_slider](Touchable *, Point p) {
        std::cout << "contrast changing" << std::endl;
        temp_contrast = (float)(contrast_slider.chosen_value) / (float)(contrast_slider.max - contrast_slider.min) * 255;

    });

    Slider saturation_slider(graphics, {30, 90}, {130, 105},
                             SimpleGraphics::rgba(0, 0, 0, 255),
                             SimpleGraphics::rgba(163, 163, 163, 255), 0, 100, sliderFont);
    saturation_slider.onTouch([&temp_saturation, &saturation_slider](Touchable *, Point p) {
        std::cout << "saturation changing" << std::endl;
        temp_saturation = (float)(saturation_slider.chosen_value) / (float)(saturation_slider.max - saturation_slider.min) * 255;
    });

    // Button back(graphics, touch, {0, 0}, {27, 120}, "exit",
    //             SimpleGraphics::rgba(255, 255, 255, 255),
    //             SimpleGraphics::rgba(100, 100, 100, 255), buttonFont);

    // back.onTouch([&sc](Point p) {
    //     std::cout << "BACK" << std::endl;
    //     sc.clear();
    //     sc.exit(Current_Screen(HOME));

    // });

    Button save(graphics, {133, 0}, {160, 120}, "save",
                SimpleGraphics::rgba(255, 140, 102, 255),
                SimpleGraphics::rgba(100, 100, 100, 255), buttonFont);

    save.onTouch([&temp_brightness, &temp_contrast, &temp_saturation, &sc](Point p) {
        std::cout << "SAVE" << std::endl;
        brightness = temp_brightness;
        contrast = temp_contrast;
        saturation = temp_saturation;
        sc.stop(4);
    });

    //	image_settings.addDrawable(&background);
    sc.addDrawable(&brightness_slider);
    sc.addTouchable(&brightness_slider);
    sc.addDrawable(&contrast_slider);
    sc.addTouchable(&contrast_slider);
    sc.addDrawable(&saturation_slider);
    sc.addTouchable(&saturation_slider);
    sc.addDrawable(&back);
    sc.addTouchable(&back);
    sc.addDrawable(&save);
    sc.addTouchable(&save);

    sc.draw();

    sc.enable_touch();

    graphics.draw_string(SimpleGraphics::rgba(0, 0, 0, 255), 43, 5, "IMAGE SETTINGS");

    graphics.draw_string(SimpleGraphics::rgba(0, 0, 0, 255), 55, 20, "BRIGHTNESS");
    graphics.draw_string(SimpleGraphics::rgba(0, 0, 0, 255), 60, 50, "CONTRAST");
    graphics.draw_string(SimpleGraphics::rgba(0, 0, 0, 255), 55, 80, "SATURATION");
    return = sc.run()
}

int showGestureRecognition(SimpleGraphics &sg, GeometricRecognizer &gr, NIOS_Processor &nios, TouchControl &tc, FontType menuFont)
{

    screen sc(sg, tc, wc);

    addDropDownMenu(sg, sc, FontType menuFont);

    //Button startBtn(sg, {320,380}, {420, 450}, "Start", rgb(0,0,0), rgb(119,119,119));
    //Button endBtn(sg, {460,380}, {560, 450}, "End", rgb(0,0,0), rgb(119,119,119));

    Path2D newPath;

    Point last_min, last_max;

    nios.dot_location_cb([&last_min, &last_max, &sg, &newPath](auto dot) {
        sg.draw_rect(rgba(0, 0, 0, 0), last_min, last_max);
        std::cout << "Point: " << dot.avg_x << ',' << dot.avg_y << std::endl;
        newPath.emplace_back(dot.avg_x, dot.avg_y);
        last_min = {dot.min_x, dot.min_y};
        last_max = {dot.max_x, dot.max_y};
        sg.draw_rect(rgba(0, 255, 0, 128), last_min, last_max);
    });

    // startBtn.onTouch([&nios](Point point){
    //     nios.start();
    // });

    // endBtn.onTouch([&nios, &newPath, &gr](Point point){
    //     std::cout << "Number of points: " << newPath.size() << std::endl;
    //     nios.stop();
    //                                                                 //  #############################################  //
    //     RecognitionResult result = gr.recognize(newPath);           //  this part will be replaced with mistery class  //
    //                                                                 //  #############################################  //
    //     std::cout << "the gesture input is: " << result.name << std::endl;
    // });

    if (wandMode == gestureMode && cmds == wandStart)
    {
    }
    nios.start();

    else if (wandMode == gestureMode && cmds == wandStop)
    {
        std::cout << "Number of points: " << newPath.size() << std::endl;
        nios.stop();
        //  #############################################  //
        RecognitionResult result = gr.recognize(newPath); //  this part will be replaced with mistery class  //
                                                        //  #############################################  //
        std::cout << "the gesture input is: " << result.name << std::endl;
    }

    std::cout << "Starting..." << std::endl;

    while (true)
    {
        m_nios.trypoll();
        wand.trypoll();
    }

    sc.addTouchable(&Menu)
        sc.addTouchable(&startBtn);
    sc.addTouchable(&endBtn);
    sc.addDrawable(&Menu);
    sc.addDrawable(&startBtn);
    sc.addDrawable(&endBtn);

    sc.draw();

    return sc.run();
}

int showHomePage(SimpleGraphics &sg, TouchControl &tc, std::string username, FontType menuFont, FontType buttonFont)
{

    screen sc(sg, tc, wc);;

    addDropDownMenu(sg, sc, FontType menuFont);

    Button welcomeField(sg, {340, 40}, {600, 80}, "Welcome!" + username, rgba(255, 255, 255, 255), rgba(159, 159, 159, 255), buttonFont);

    sc.addDrawable(&homeMenu);
    sc.addDrawable(&welcomeField);
    sc.addTouchable(&homeMenu);
    sc.addTouchable(&welcomeField);

    sc.draw();

    return sc.run();
}

int main(void)
{

    // Simplegraphics
    SimpleGraphics sg(640, 480);
    sg.clear();

    // NIOS
    FIFO_Serial nios_serial(NIOS_HPS_FIFO_BASE, NIOS_HPS_FIFO_STATUS_BASE,
                            HPS_NIOS_FIFO_BASE, HPS_NIOS_FIFO_STATUS_BASE);

    NIOS_Processor nios(nios_serial);

    // Touch
    TouchControl tc("/dev/ttyAL0", sg.width(), sg.height(), true);
    tc.touch_enable();
    // Wifi
    Wifi wifi("/dev/ttyAL1");
    // Videp
    Video video("/dev/ttyAL2");
    // Bluetooth
    Bluetooth bt("/dev/ttyAL3");
    // Gesture Recognizer
    GeometricRecognizer gr;
    gr.loadSamples();

    int scret = showStartScreen(sg);

    while (1)
    {
        switch (scret):
            {
            case to_Home:
            {
                scret = showHomePage(sg, "group 5");
                break;
            }
            case to_LoginPanel:
            {
                scret = showLoginPanel();
                break;
            }
            case to_GestureMap:
            {
                scret = showGestureRecognition(sg, gr, nios);
                break;
            }
            }
    }

    return 0;
}
