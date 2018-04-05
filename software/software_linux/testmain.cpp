
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
#include "Animation.hpp"
//#include "LoginPanel.hpp"

using namespace UI;
using namespace DollarRecognizer;
using namespace SimpleGraphicsFonts;

enum Switch_Page_Commands
{
    to_Home = 0,
    to_GestureMap = 2,
    to_NewGesture = 3,
    to_Setting = 4,
    to_LoginPanel = 5
};

// void showStartScreen(SimpleGraphics &sg, TouchControl &tc, NIOS_Processor &nios, Wand &wand) {

//     WandControl wc(wand, nios);

//     Screen screen(sg, tc, wc);

//     auto doneCB = [&screen] () {
//         std::cout << "Done" << std::endl;
//         screen.stop(0);
//     };
//     auto progressCB = [&sg] (unsigned progress) {
//         sg.draw_rect(rgb(0, 0, 0), 100, 100, 500, 160);
//         std::ostringstream s;
//         s << "Progress: " << progress << '%';
//         sg.draw_string(rgb(255, 255, 255), 100, 100, s.str(), Font38x59);
//     };

// }

// void addDropDownMenu(SimpleGraphics &sg, Screen &sc, FontType menuFont){
//     DropdownMenu homeMenu(sg, {40, 40}, {240, 100}, "Menu", rgba(0, 0, 0, 255), rgba(159, 159, 159, 255), menuFont);
//     homeMenu.newItem(sg, "Home", [&sc](Point p) {
//                 sc.stop(0);
//     });

//     homeMenu.newItem(sg, "Gesture Map", [&sc](Point p) {
//                 sc.stop(2);
//     });

//     homeMenu.newItem(sg, "New Gesture", [&sc](Point p) {
//                 sc.stop(3);
//     });

//     homeMenu.newItem(sg, "Settings", [&sc](Point p) {
//                 sc.stop(4);
//     });

//     homeMenu.newItem(sg, "Log Out", [&sc](Point p) {
//                 sc.stop(5);
//     });

//     // NIOS_Processor_Init init(SDRAM_FILE, SDRAM_BASE, SDRAM_SPAN, 
//     //         MM_RESET_BASE, MM_RESET_SPAN, doneCB, progressCB);

//     //init.run(screen);

//     sc.addDrawable(&homeMenu);
//     sc.addTouchable(&homeMenu);
//}

/*
 * @Param:
 *      username:  callback from login() is login successfully
 *      mistery class reference: implement on Tuesday
 */
// int showLoginPanel(SimpleGraphics &sg, Wifi &wifi, Wand &wand, GeometricRecognizer &gr, Video &video, NIOS_Processor &nios)
// {

//     WandControl wc(wand, nios);

//     // lp is a subclass of screen
//     LoginPanel lp(sg, wifi, video, wc, {0, 0}, {640, 480}, Font16x27, Font10x14);

//     return lp.run();
// }

int showSetting(SimpleGraphics &sg, Wand &wand, NIOS_Processor &nios, FontType buttonFont, FontType sliderFont, FontType menuFont, TouchControl &tc, Video &video)
{   
    WandControl wc(wand, nios);

    Screen sc(sg, tc, wc);

    int temp_brightness;
    int temp_contrast;
    int temp_saturation;

    int brightness, contrast, saturation;

    Rectangle background_colour(sg, {0,0}, {640,480}, rgba(255, 255, 255, 255));

    Button title(sg, {0,0}, {640, 100}, "IMAGE SETTINGS", rgba(0, 0, 0, 255), rgba(255, 255, 255, 255), Font22x40);

    Slider brightness_slider(sg, {sg.width()/4, 120}, {3*sg.width()/4, 160},
                             rgba(0, 0, 0, 255), rgba(163, 163, 163, 255), 0, 100, sliderFont);

    Slider contrast_slider(sg, {sg.width()/4, 200}, {3*sg.width()/4, 240},
                           rgba(0, 0, 0, 255), rgba(163, 163, 163, 255), 0, 100, sliderFont);

    Slider saturation_slider(sg, {sg.width()/4, 280}, {3*sg.width()/4, 320},
                             rgba(0, 0, 0, 255), rgba(163, 163, 163, 255), 0, 100, sliderFont);


    // when sliders are touched
    brightness_slider.onTouch([&temp_brightness, &brightness_slider](Point p) {
        temp_brightness = (float)(brightness_slider.value()) / (float)(brightness_slider.getMax() - brightness_slider.getMin()) * 127;
    });

    contrast_slider.onTouch([&temp_contrast, &contrast_slider](Point p) {
        temp_contrast = (float)(contrast_slider.value()) / (float)(contrast_slider.getMax() - contrast_slider.getMin()) * 255;
    });

    saturation_slider.onTouch([&temp_saturation, &saturation_slider](Point p) {
        temp_saturation = (float)(saturation_slider.value()) / (float)(saturation_slider.getMax() - saturation_slider.getMin()) * 255;
    });

    // Button back(graphics, touch, {0, 0}, {27, 120}, "exit",
    //             SimpleGraphics::rgba(255, 255, 255, 255),
    //             SimpleGraphics::rgba(100, 100, 100, 255), buttonFont);

    // back.onTouch([&sc](Point p) {
    //     std::cout << "BACK" << std::endl;
    //     sc.clear();
    //     sc.exit(Current_Screen(HOME));

    // });

    Button save(sg, {sg.width()/3, 400}, {2*sg.width()/3, sg.height()-30}, "Save",
                rgba(255, 140, 102, 255), rgba(100, 100, 100, 255), buttonFont);

    save.onTouch([&temp_brightness, &temp_contrast, &temp_saturation, &sc, &brightness, &contrast, &saturation, &video](Point p) {
        std::cout << "SAVE" << std::endl;
        brightness = temp_brightness;
        contrast = temp_contrast;
        saturation = temp_saturation;
        video.imageSettings(brightness, contrast, 0, 0, saturation);
        sc.stop(4);
    });

    // Exit Button
    Button exit(sg, {2*sg.width()/3+130, sg.height()-40}, {sg.width()-10, sg.height()-10}, "Exit", rgba(255, 255, 255, 255), rgba(204, 63, 63, 255), Font16x27);
    
    exit.onTouch([&sc](Point p) {
        sc.stop(4);
    });


    Button brightness_label(sg, {sg.width()/4, 100}, {3*sg.width()/4, 120}, "BRIGHTNESS", rgba(0, 0, 0, 255), rgba(255, 255, 255, 255), Font10x14);
    Button contrast_label(sg, {sg.width()/4, 180}, {3*sg.width()/4, 200}, "CONTRAST", rgba(0, 0, 0, 255), rgba(255, 255, 255, 255), Font10x14);
    Button saturation_label(sg, {sg.width()/4, 260}, {3*sg.width()/4, 280}, "SATURATION", rgba(0, 0, 0, 255), rgba(255, 255, 255, 255), Font10x14);

    //	image_settings.addDrawable(&background);
    sc.addDrawable(&background_colour);
    sc.addDrawable(&title);
    sc.addDrawable(&brightness_slider);
    sc.addTouchable(&brightness_slider);
    sc.addDrawable(&contrast_slider);
    sc.addTouchable(&contrast_slider);
    sc.addDrawable(&saturation_slider);
    sc.addTouchable(&saturation_slider);
    //sc.addDrawable(&back);
    //sc.addTouchable(&back);
    sc.addDrawable(&save);
    sc.addTouchable(&save);
    sc.addDrawable(&exit);
    sc.addTouchable(&exit);
    sc.addDrawable(&brightness_label);
    sc.addDrawable(&contrast_label);
    sc.addDrawable(&saturation_label);

    sc.draw();

    return sc.run();
}

// int showGestureRecognition(SimpleGraphics &sg, GeometricRecognizer &gr, NIOS_Processor &nios, TouchControl &tc, Wand &wand, FontType menuFont)
// {

//     WandControl wc(wand, nios);

//     screen sc(sg, tc, wc);

//     addDropDownMenu(sg, sc, FontType menuFont);

//     Button startBtn(sg, {320,380}, {420, 450}, "Start", rgb(0,0,0), rgb(119,119,119));
//     Button endBtn(sg, {460,380}, {560, 450}, "End", rgb(0,0,0), rgb(119,119,119));

//     Path2D newPath;

//     //Point last_min, last_max;

//     // nios.dot_location_cb([&last_min, &last_max, &sg, &newPath](auto dot) {
//     //     sg.draw_rect(rgba(0, 0, 0, 0), last_min, last_max);
//     //     std::cout << "Point: " << dot.avg_x << ',' << dot.avg_y << std::endl;
//     //     newPath.emplace_back(dot.avg_x, dot.avg_y);
//     //     last_min = {dot.min_x, dot.min_y};
//     //     last_max = {dot.max_x, dot.max_y};
//     //     sg.draw_rect(rgba(0, 255, 0, 128), last_min, last_max);
//     // });

//     // startBtn.onTouch([&nios](Point point){
//     //     nios.start();
//     // });

//     // endBtn.onTouch([&nios, &newPath, &gr](Point point){
//     //     std::cout << "Number of points: " << newPath.size() << std::endl;
//     //     nios.stop();
//     //                                                                 //  #############################################  //
//     //     RecognitionResult result = gr.recognize(newPath);           //  this part will be replaced with mistery class  //
//     //                                                                 //  #############################################  //
//     //     std::cout << "the gesture input is: " << result.name << std::endl;
//     // });

//     std::cout << "Starting..." << std::endl;

//     while (true)
//     {
//         m_nios.trypoll();
//         wand.trypoll();
//     }

//     sc.addTouchable(&Menu)
//     sc.addTouchable(&startBtn);
//     sc.addTouchable(&endBtn);
//     sc.addDrawable(&Menu);
//     sc.addDrawable(&startBtn);
//     sc.addDrawable(&endBtn);

//     sc.draw();

//     return sc.run();
// }

int showHomePage(SimpleGraphics &sg, NIOS_Processor &nios, TouchControl &tc, Wand &wand, std::string username, FontType menuFont, FontType buttonFont)
{

    WandControl wc(wand, nios);

    Screen sc(sg, tc, wc);



    //addDropDownMenu(sg, sc, menuFont);
    DropdownMenu homeMenu(sg, {0, 60}, {240, 110}, rgba(0, 0, 0, 255), rgba(159, 159, 159, 255), menuFont);
    homeMenu.newItem(sg, "Home", [&sc](Point p) {
                sc.stop(0);
    });

    homeMenu.newItem(sg, "Gesture Map", [&sc](Point p) {
                sc.stop(2);
    });

    homeMenu.newItem(sg, "New Gesture", [&sc](Point p) {
                sc.stop(3);
    });

    homeMenu.newItem(sg, "Settings", [&sc](Point p) {
                sc.stop(4);
    });

    homeMenu.newItem(sg, "Log Out", [&sc](Point p) {
                sc.stop(5);
    });

    // NIOS_Processor_Init init(SDRAM_FILE, SDRAM_BASE, SDRAM_SPAN, 
    //         MM_RESET_BASE, MM_RESET_SPAN, doneCB, progressCB);

    //init.run(screen);

    // Slide in animation
    Animation<> fade_in(60, 310, 20);
    Animation<> fade_out(60, 310, 20);
    
    fade_in.cb([&sg, &sc, &homeMenu] (int val, bool end) {
        sg.draw_rect(rgba(159, 159, 159, 255), {0, 60}, {240, (unsigned)val});
        
        if (end) {
            homeMenu.expand();
        }
    });

    fade_out.cb([&sg, &sc, &homeMenu] (int val, bool end) {
        sg.draw_rect(rgba(0, 0, 0, 0), {0,370-(unsigned)val}, {240,370});
        
        if (end) {
            homeMenu.close();
        }
    });

    Button dropdownButton(sg, {0,0}, {340,60}, "Menu", rgba(0, 0, 0, 255), rgba(170,170,170,255), menuFont);

    dropdownButton.onTouch([&homeMenu, &sc, &fade_out, &fade_in](Point p){
        if (homeMenu.isOpen()){
            fade_out.run(sc);
        }
        else {
            fade_in.run(sc);
        }
    });

    Button welcomeField(sg, {340, 0}, {640, 60}, "Welcome!" + username, rgba(255, 255, 255, 255), rgba(159, 159, 159, 255), buttonFont);

    sc.addDrawable(&dropdownButton);
    sc.addTouchable(&dropdownButton);
    sc.addDrawable(&homeMenu);
    sc.addTouchable(&homeMenu);
    sc.addDrawable(&welcomeField);
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
    //video.mirror_mode_on();
    // Bluetooth
    Wand wand("/dev/ttyAL3", B115K);
    // Gesture Recognizer
    GeometricRecognizer gr;
    gr.loadSamples();

    //int scret = showHomePage(sg, nios, tc, wand, "", Font22x40, Font16x27);

    while (1)
    {
        // switch (scret){
        //     case to_Home:
        //     {
        //         scret = showHomePage(sg, nios, tc, wand, "", Font22x40, Font16x27);
        //         break;
        //     }
        //     case to_LoginPanel:
        //     {
        //         scret = showHomePage(sg, nios, tc, wand, "", Font22x40, Font16x27);
        //         break;
        //     }
        //     case to_GestureMap:
        //     {
        //         scret = showHomePage(sg, nios, tc, wand, "", Font22x40, Font16x27);
        //         break;
        //     }
        // }
        //showHomePage(sg, nios, tc, wand, "", Font22x40, Font16x27);
        showSetting(sg, wand, nios, Font22x40, Font10x14, Font16x27, tc, video);
    }

    return 0;
}
