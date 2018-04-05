
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
#include "LoginPanel.hpp"

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

void addDropDownMenu(SimpleGraphics &sg, screen &sc, FontType menuFont){
    DropdownMenu homeMenu(sg, {40, 40}, {180, 100}, "Menu", rgba(0, 0, 0, 255), rgba(159, 159, 159, 255), menuFont);
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

    // what does this do ???????????
    NIOS_Processor_Init init(SDRAM_FILE, SDRAM_BASE, SDRAM_SPAN, 
            MM_RESET_BASE, MM_RESET_SPAN, doneCB, progressCB);

    init.run(sc);

    
    sc.addDrawable(homeMenu);
    sc.addTouchable(homeMenu);

    sc.run();
}

void showStartScreen(SimpleGraphics &sg, TouchControl &tc, NIOS_Processor &nios, Wand &wand) {

    WandControl wc(wand, nios);

    Screen screen(sg, tc, wc);

    auto doneCB = [&screen] () {
        std::cout << "Done" << std::endl;
        screen.stop(0);
    };
    auto progressCB = [&sg] (unsigned progress) {
        sg.draw_rect(rgb(0, 0, 0), 100, 100, 500, 160);
        std::ostringstream s;
        s << "Progress: " << progress << '%';
        sg.draw_string(rgb(255, 255, 255), 100, 100, s.str(), Font38x59);
    };
}

/*
 * @Param:
 *      username:  callback from login() is login successfully
 *      mistery class reference: implement on Tuesday
 */
int showLoginPanel(SimpleGraphics &sg, TouchControl &tc, NIOS_Processor &nios, Wand &wand, Wifi &wifi, GeometricRecognizer &gr, Video &video)
{
    WandControl wc(wand, nios);

    // lp is a subclass of screen
    LoginPanel lp(sg, wifi, video, wc, {0, 0}, {640, 480}, Font16x27, Font10x14);

    return lp.run();
}

int showSetting(SimpleGraphics &sg, TouchControl &tc, NIOS_Processor &nios, Wand &wand, FontType sliderFont, FontType menuFont)
{   
    WandControl wc(wand, nios);

    screen sc(sg, tc, wc);

    addDropDownMenu(sg, sc, FontType menuFont);

    int temp_brightness;
    int temp_contrast;
    int temp_saturation;

    Slider brightness_slider(graphics, {320, 120}, {600, 160},
                             rgba(0, 0, 0, 255), rgba(163, 163, 163, 255), 0, 100, sliderFont);

    Slider contrast_slider(graphics, {320, 200}, {600, 240},
                           rgba(0, 0, 0, 255), rgba(163, 163, 163, 255), 0, 100, sliderFont);

    Slider saturation_slider(graphics, {320, 280}, {600, 320},
                             rgba(0, 0, 0, 255), rgba(163, 163, 163, 255), 0, 100, sliderFont);


    // when sliders are touched
    brightness_slider.onTouch([&temp_brightness, &brightness_slider](Point p) {
        std::cout << "brightness changing" << std::endl;
        temp_brightness = (float)(brightness_slider.chosen_value) / (float)(brightness_slider.max - brightness_slider.min) * 127;
    });

    contrast_slider.onTouch([&temp_contrast, &contrast_slider](Point p) {
        std::cout << "contrast changing" << std::endl;
        temp_contrast = (float)(contrast_slider.chosen_value) / (float)(contrast_slider.max - contrast_slider.min) * 255;
    });

    saturation_slider.onTouch([&temp_saturation, &saturation_slider](Point p) {
        std::cout << "saturation changing" << std::endl;
        temp_saturation = (float)(saturation_slider.chosen_value) / (float)(saturation_slider.max - saturation_slider.min) * 255;
    });

    // ######################## //
    // when sliders are clicked //
    // ???????????????????????? //
    // ######################## //
    sc.m_wandControl.setCursorClickCB([&this, &temp_saturation, &saturation_slider](Point p){
        if(brightness_slider.touch(p)) {
            std::cout << "brightness changing" << std::endl;
            temp_brightness = (float)(brightness_slider.chosen_value) / (float)(brightness_slider.max - brightness_slider.min) * 127;
        }
        else if(contrast_slider.touch(p){
            std::cout << "contrast changing" << std::endl;
            temp_contrast = (float)(contrast_slider.chosen_value) / (float)(contrast_slider.max - contrast_slider.min) * 255;
        })  
        else if(saturation_slider.touch(p)){
            std::cout << "contrast changing" << std::endl;
            temp_contrast = (float)(contrast_slider.chosen_value) / (float)(contrast_slider.max - contrast_slider.min) * 255;
        }
    });

    // Button back(graphics, touch, {0, 0}, {27, 120}, "exit",
    //             SimpleGraphics::rgba(255, 255, 255, 255),
    //             SimpleGraphics::rgba(100, 100, 100, 255), buttonFont);

    // back.onTouch([&sc](Point p) {
    //     std::cout << "BACK" << std::endl;
    //     sc.clear();
    //     sc.exit(Current_Screen(HOME));

    // });

    Button save(graphics, {520, 360}, {600, 400}, "Save",
                rgba(255, 140, 102, 255), rgba(100, 100, 100, 255), buttonFont);

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
    //sc.addDrawable(&back);
    //sc.addTouchable(&back);
    sc.addDrawable(&save);
    sc.addTouchable(&save);

    sc.draw();

    //graphics.draw_string(rgba(0, 0, 0, 255), 322, 100, "IMAGE SETTINGS", Font10x14);
    graphics.draw_string(rgba(0, 0, 0, 255), 322, 100, "BRIGHTNESS", Font10x14);
    graphics.draw_string(rgba(0, 0, 0, 255), 322, 180, "CONTRAST", Font10x14);
    graphics.draw_string(rgba(0, 0, 0, 255), 322, 260, "SATURATION", Font10x14);

    return sc.run();
}

int showGestureMapping(SimpleGraphics &sg, TouchControl &tc, NIOS_Processor &nios, Wand &wand, Wifi &wifi, FontType menuFont, FontType buttonfont)
{
    WandControl wc(wand, nios);

    screen sc(sg, tc, wc);

    std::unordered_map<std::string, std::string> service_map(wifi.GetGestureMapping());
    std::unordered_map<std::string, std::string> local_map(wifi.GetGestureMapping());
    std::vector<Button> service_list;

    bool select_service = false;
    bool receive_gesture = true;

    int i = 0;
    int service_btn_height = 40;
    int top_service_btn_ul_y = 100;

    char rcv_char = '';

    // put all service we have into vector<Button>
    for(std::map<std::string, std::string>::iterator it = service_map.begin(); it != service_map.end(); ++it){

        Button btn(sg, {500, top_service_btn_ul_y + i*service_btn_height}, 
                   {600, top_service_btn_ul_y + (i+1)*service_btn_height}, it->second,
                   rgba(53,53,53,255), rgba(191,191,191,255), Font16x27 = 14,);

        service_list.push_back(btn);  
        i++;
    }
    
    Button service1 = service_list.back(); service_list.pop_back();
    Button service2 = service_list.back(); service_list.pop_back();
    Button service3 = service_list.back(); service_list.pop_back();
    Button service4 = service_list.back(); service_list.pop_back();
    Button service5 = service_list.back(); service_list.pop_back();

    Button finish_gesture_and_sel_service(sg, {220, 380}, {320, 430}, "Confirm Gesture", 
                                          rgba(53,53,53,255), rgba(191,191,191,255), Font16x27);
    Button retype_gesture(sg, {360, 380}, {460, 430}, "Retype Gesture", 
                          rgba(53,53,53,255), rgba(191,191,191,255), Font16x27);
    Button input_recognition_field(sg, {220, 310}, {460, 350}, "Input Gesture: ", 
                                   rgba(53,53,53,255), rgba(255,255,255,255), Font16x27);
    Button touch_field(sg, {220, 60}, {460, 280}}, "", 
                            rgba(53,53,53,255), rgba(255,255,255,255), Font16x27);

    
    // sc.setTypingCB([&input_recognition_field, &rcv_char](char c){

    //     rcv_char = c; 
    //     input_recognition_field = Button(sg, {220, 310}, {460, 350}, "Input Gesture: " + rcv_char, 
    //                                rgba(53,53,53,255), rgba(255,255,255,255), Font16x27);
    // });

    touch_field.onTouch([&sg, &receive_gesture](Point p){
        if(receive_gesture)
            sg.draw_rect(rgba(255,0,0,255), {point.x, point.y}, {point.x+2, point.y+2});
    });


    sc.addDrawable(service1);
    sc.addDrawable(service2);
    sc.addDrawable(service3);
    sc.addDrawable(service4);
    sc.addDrawable(service5);
    
    sc.addDrawable(finish_gesture_and_sel_service);
    sc.addDrawable(retype_gesture);
    sc.addDrawable(input_recognition_field);  
    sc.addDrawable(touch_field);    

    sc.addTouchable(service1);
    sc.addTouchable(service2);
    sc.addTouchable(service3);
    sc.addTouchable(service4);
    sc.addTouchable(service5);
    
    sc.addTouchable(finish_gesture_and_sel_service);
    sc.addTouchable(retype_gesture);
    sc.addTouchable(input_recognition_field);  
    sc.addTouchable(touch_field);    

    


    sc.draw();
    
    return sc.run();
}



int showHomePage(SimpleGraphics &sg, TouchControl &tc, NIOS_Processor &nios, Wand &wand, std::string username, FontType menuFont, FontType buttonFont)
{

    WandControl wc(wand, nios);

    screen sc(sg, tc, wc);

    addDropDownMenu(sg, sc, FontType menuFont);

    Button welcomeField(sg, {340, 40}, {600, 80}, "Welcome!" + username, rgba(255, 255, 255, 255), rgba(159, 159, 159, 255), buttonFont);

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
    // Bluetooth
    Wand wand("/dev/ttyAL3");
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
                scret = showHomePage(sg, tc, wand, "Group 5", Font22x40, Font16x27);
                break;
            }
            case to_LoginPanel:
            {
                scret = showLoginPanel(sg, tc, nios, wand, wifi, gr, video);
                break;
            }
            case to_GestureMap:
            {
                scret = showGestureMapping(sg, tc, nios, wand, gr, Font22x40);
                break;
            }
            case to_Setting:
            {
                scret = showSetting(sg, tc, nios, wand, Font16x27, Font22x40);
            }
        }
    }

    return 0;
}
