#include <system_error>
#include <unistd.h>
#include <iostream>
#include <memory>
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
#include "bluetooth.hpp"
#include "cursor.hpp"

using namespace UI;
using namespace DollarRecognizer;
using namespace SimpleGraphicsFonts;


enum Switch_Page_Commands : int
{
    to_Home = 0,
    to_Bluetooth = 1,
    to_FacialLogin = 2,
    to_UsernameLogin = 3,
    to_Setting = 4
};


void showStartScreen(SimpleGraphics &sg, TouchControl &tc, GeometricRecognizer &geo, Wand &wand, NIOS_Processor &nios, Cursor &gesture_cursor, Cursor &typing_cursor, Cursor &mouse_cursor, FIFO_Serial &fifo) {
    std::cout << "Start Screen Running" << std::endl;
     NIOS_Processor_Init nios_init(SDRAM_FILE, SDRAM_BASE, SDRAM_SPAN,
             MM_RESET_BASE, MM_RESET_SPAN);

     nios_init.printCB([&sg] (std::string str) {
         sg.draw_string_bg_centered(rgb(255, 255, 255), sg.width() / 2, 400, 36, Font16x27);
         sg.draw_string_centered(rgb(0, 0, 255), sg.width() / 2, 400, str, Font16x27);
     });
     nios_init.progressCB([&sg] (unsigned progress) {
         std::cout << "Progress: " << progress << '%' << std::endl;
         sg.draw_rect(rgb(255, 0, 0), {220, 300}, {2 * progress + 220, 340});
         sg.draw_rect(rgb(0, 0, 0), {2*progress + 220, 300}, {420, 340});
     });

     Screen screen(sg, tc, geo, wand, nios, gesture_cursor, typing_cursor, mouse_cursor);

     nios_init.doneCB([&nios, &screen] () {
        if(nios.hello(250)) {
            std::clog << "Cannot initialize NIOS" << std::endl;
            assert(0);
        }

        screen.stop(0);
     });

     screen.add_timer(std::chrono::milliseconds(250), [&sg, &nios_init, &fifo] (Event_Loop *ev) -> bool {
         sg.fill(rgb(255, 255, 255));
         sg.draw_logo("logo.bmp", 160, 100);

         std::cout << "Starting NIOS INIT" << std::endl;
         fifo.clear();
         nios_init.run(*ev);

         return false;
     });

     screen.draw();
     screen.run();
 }


int showSetting(SimpleGraphics &sg, TouchControl &tc, GeometricRecognizer &geo, Wand &wand, NIOS_Processor &nios, Cursor &gesture_cursor, Cursor &typing_cursor, Cursor &mouse_cursor, Video &video)
{   

    std::cout << "Setting Screen Running" << std::endl;
    Screen sc(sg, tc, geo, wand, nios, gesture_cursor, typing_cursor, mouse_cursor);

    int temp_brightness;
    int temp_contrast;
    int temp_saturation;

    int brightness, contrast, saturation;

    Rectangle background_colour(sg, {0,0}, {640,480}, rgba(255, 255, 255, 255));

    Button title(sg, {0,0}, {640, 100}, "IMAGE SETTINGS", rgba(0, 0, 0, 255), rgba(255, 255, 255, 255), Font22x40);

    Slider brightness_slider(sg, {sg.width()/4, 120}, {3*sg.width()/4, 160},
                             rgba(0, 0, 0, 255), rgba(163, 163, 163, 255), 0, 100);

    Slider contrast_slider(sg, {sg.width()/4, 200}, {3*sg.width()/4, 240},
                           rgba(0, 0, 0, 255), rgba(163, 163, 163, 255), 0, 100);

    Slider saturation_slider(sg, {sg.width()/4, 280}, {3*sg.width()/4, 320},
                             rgba(0, 0, 0, 255), rgba(163, 163, 163, 255), 0, 100);


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

    Button save(sg, {sg.width()/3, 400}, {2*sg.width()/3, sg.height()-30}, "Save",
                rgba(255, 140, 102, 255), rgba(100, 100, 100, 255), Font16x27);

    save.onTouch([&temp_brightness, &temp_contrast, &temp_saturation, &sc, &brightness, &contrast, &saturation, &video](Point p) {
        std::cout << "SAVE" << std::endl;
        brightness = temp_brightness;
        contrast = temp_contrast;
        saturation = temp_saturation;
        video.imageSettings(brightness, contrast, 0, 0, saturation);
        sc.stop(to_Home);
    });

    // Exit Button
    Button exit(sg, {2*sg.width()/3+130, sg.height()-40}, {sg.width()-10, sg.height()-10}, "Exit", rgba(255, 255, 255, 255), rgba(204, 63, 63, 255), Font16x27);
    
    exit.onTouch([&sc](Point p) {
        sc.stop(to_Home);
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
    sc.start();

    int ret = sc.run();

    nios.stop();

    return ret;
}

int showBluetoothPage(SimpleGraphics &sg, TouchControl &tc, GeometricRecognizer &geo, Wand &wand, NIOS_Processor &nios, Cursor &gesture_cursor, Cursor &typing_cursor, Cursor &mouse_cursor){

    std::cout << "Bluetooth Screen Running" << std::endl;
    Screen sc(sg, tc, geo, wand, nios, gesture_cursor, typing_cursor, mouse_cursor);
    
    std::string selected_device_str = "";
    
    std::vector<std::pair<std::string, std::string>> available_devices;
    std::vector<std::pair<std::unique_ptr<Button>,std::unique_ptr<Button>>> device_buttons;

    Rectangle background_colour(sg, {0,0}, {640,480}, rgba(255, 255, 255, 255));
    
    // Page Label: Bluetooth Settings
    Button title(sg, {10,10}, {400,40}, "Bluetooth Settings", rgba(0, 0, 0, 255), rgba(255, 255, 255, 255), Font22x40);
    
    // Button to find available bluetooth devices
    Button findDevices(sg, {10, 60}, {sg.width()/2, 100}, "Find Available Devices", rgba(255, 255, 255, 255), rgba(58, 60, 193, 255), Font16x27);
    
    // Slide in animation
    Animation<> fade_in(160, 180+20*available_devices.size(), 5);
    
    fade_in.cb([&sg] (int val, bool end) {
        sg.draw_rect(rgba(200, 200, 200, 255), {10, 160}, {sg.width()-10, (unsigned)val});
        
        if (end) {
            std::cout << "Slide In Done" << std::endl;
        }
    });
    
    findDevices.onTouch([&available_devices, &wand, &sg, &fade_in, &sc, &selected_device_str, &device_buttons](Point p) {
        available_devices = wand.getAvailableDevices("7");
        
        fade_in.run(sc);
        
        // Available bluetooth devices
        for (int i = 0; i < (int)available_devices.size(); i++) {
            Button *b1 = new Button(sg, {10, 160+20*(unsigned)i}, {sg.width()/2, 180+20*(unsigned)i}, available_devices[i].first, rgba(0, 0, 0, 255), rgba(200, 200, 200, 255), Font10x14);

            Button *b2 = new Button(sg, {sg.width()/2, 160+20*(unsigned)i}, {sg.width()-10, 180+20*(unsigned)i}, available_devices[i].second, rgba(0, 0, 0, 255), rgba(200, 200, 200, 255), Font10x14);

            device_buttons.emplace_back(std::unique_ptr<Button>(b1), std::unique_ptr<Button>(b2));
            
            sc.addDrawable(device_buttons[i].second.get());
            sc.addTouchable(device_buttons[i].second.get());
            sc.addDrawable(device_buttons[i].first.get());
            
            device_buttons[i].first->draw();
            device_buttons[i].second->draw();
            
            device_buttons[i].second->onTouch([&sg, &selected_device_str, &available_devices, &i, &sc](Point p) {
                selected_device_str = available_devices[i].second;
                Button *device = new Button(sg, {282,sg.height()-40}, {2*sg.width()/3,sg.height()-10}, selected_device_str, rgba(0, 0, 0, 255), rgba(255, 255, 255, 255), Font16x27);

                device->draw();

                sc.addDrawable(device);
            });
        }
        
    });
    
    // Table Headers
    Button name(sg, {10,120}, {sg.width()/2,136}, "Name", rgba(0, 0, 0, 255), rgba(255, 255, 255, 255), Font16x27);
    Button address(sg, {sg.width()/2,120}, {sg.width(),136}, "Address", rgba(0, 0, 0, 255), rgba(255, 255, 255, 255), Font16x27);
    
    // Label: Selected Device:
    Button selectedDevice(sg, {10,sg.height()-40}, {282,sg.height()-10}, "Selected Device: ", rgba(0, 0, 0, 255), rgba(255, 255, 255, 255), Font16x27);
    
    // Connect Button
    Button connect(sg, {2*sg.width()/3, sg.height()-40}, {2*sg.width()/3+120, sg.height()-10}, "Connect", rgba(255, 255, 255, 255), rgba(204, 63, 63, 255), Font16x27);
    
    connect.onTouch([&wand, &selected_device_str](Point p) {
        wand.connectToAddress(selected_device_str);
    });
    
    // Exit Button
    Button exit(sg, {2*sg.width()/3+130, sg.height()-40}, {sg.width()-10, sg.height()-10}, "Exit", rgba(255, 255, 255, 255), rgba(204, 63, 63, 255), Font16x27);
    
    exit.onTouch([&sc](Point p) {
        sc.stop(to_Home);
    });


    sc.addDrawable(&background_colour);
    sc.addDrawable(&title);
    sc.addDrawable(&name);
    sc.addDrawable(&address);
    sc.addDrawable(&selectedDevice);
    sc.addTouchable(&findDevices);
    sc.addDrawable(&findDevices);
    sc.addTouchable(&connect);
    sc.addDrawable(&connect);
    sc.addTouchable(&exit);
    sc.addDrawable(&exit);

    sc.draw();
    sc.start();
    
    int ret = sc.run();

    nios.stop();

    return ret;
}

int showHomePage(SimpleGraphics &sg, TouchControl &tc, GeometricRecognizer &geo, Wand &wand, NIOS_Processor &nios, Cursor &gesture_cursor, Cursor &typing_cursor, Cursor &mouse_cursor, Wifi &wifi, std::string username) {
    std::cout << "Home Screen Running" << std::endl;

    Screen sc(sg, tc, geo, wand, nios, gesture_cursor, typing_cursor, mouse_cursor);
    auto mapping = wifi.GetGestureMapping();
    std::string descriptor;


    //addDropDownMenu(sg, sc, menuFont);
    DropdownMenu homeMenu(sg, {0, 60}, {240, 110}, rgba(0, 0, 0, 255), rgba(159, 159, 159, 255), Font16x27);
    homeMenu.newItem(sg, "Home", [&sc](Point p) {
                sc.stop(to_Home);
    });

    homeMenu.newItem(sg, "Bluetooth", [&sc](Point p) {
                sc.stop(to_Bluetooth);
    });

    homeMenu.newItem(sg, "Facial login", [&sc](Point p) {
                sc.stop(to_FacialLogin);
    });

    homeMenu.newItem(sg, "Username login", [&sc](Point p) {
                sc.stop(to_UsernameLogin);
    });

    homeMenu.newItem(sg, "Settings", [&sc](Point p) {
                sc.stop(to_Setting);
    });

    // Slide in animation
    Animation<> slide_in(60, 310, 20);
    Animation<> slide_out(370, 60, -20);

    Button dropdownButton(sg, {0,0}, {340,60}, "Menu", rgba(0, 0, 0, 255), rgba(170,170,170,255), Font22x40);

    dropdownButton.onTouch([&homeMenu, &sc, &slide_out, &slide_in](Point p){
        if (homeMenu.isOpen()){
            slide_out.run(sc);
        }
        else {
            slide_in.run(sc);
        }
    });
    
    slide_in.cb([&sg, &homeMenu] (int val, bool end) {
        sg.draw_rect(rgba(159, 159, 159, 255), {0, 60}, {240, (unsigned)val});
        
        if (end) {
            homeMenu.expand();
        }
    });

    slide_out.cb([&sg, &homeMenu, &dropdownButton] (int val, bool end) {
        sg.draw_rect(rgba(0, 0, 0, 0), {0, (unsigned) val}, {240,370});
        
        if (end) {
            homeMenu.close();
            dropdownButton.draw();
        }
    });

    sc.setGestureCB([&descriptor, &wifi, &mapping](std::string gesture) {
        descriptor = mapping[gesture];

        if (!descriptor.empty()) {
            std::cout << gesture << descriptor << std::endl;
            wifi.SendTriggeredGesture(descriptor);
        }

    });

    Button welcomeField(sg, {340, 0}, {640, 60}, "Welcome!" + username, rgba(255, 255, 255, 255), rgba(159, 159, 159, 255), Font22x40);

    sc.addDrawable(&dropdownButton);
    sc.addTouchable(&dropdownButton);
    sc.addDrawable(&homeMenu);
    sc.addTouchable(&homeMenu);
    sc.addDrawable(&welcomeField);
    sc.addTouchable(&welcomeField);

    sc.start();

    sc.draw();

    int ret = sc.run();

    nios.stop();

    return ret;
}

int showLoginPanel(SimpleGraphics &sg, TouchControl &tc, GeometricRecognizer &gr, Wand &wand, NIOS_Processor &nios, Cursor &gesture_cursor, Cursor &typing_cursor, Cursor &mouse_cursor, Wifi &wifi, Video &video, 
                   std::string &username_input)
{
    std::cout << "Login Panel Screen Running" << std::endl;

    Screen sc(sg, tc, gr, wand, nios, gesture_cursor, typing_cursor, mouse_cursor);

    Point p1(0,0);
    Point p2(640,480);

    static constexpr int BUTTON_HEIGHT = 50;
    static constexpr int BUTTON_WIDTH = 200;
    static constexpr int INPUT_FIELD_WIDTH = 300;
    std::string password_input = "";
    std::string password_display = "";
    bool input_field_chosen = false;

    Point u_f_p1 = {p1.x + (p2.x - p1.x - INPUT_FIELD_WIDTH)/2, p1.y + (p2.y - p1.y)/3};
    Point u_f_p2 = {p2.x - (p2.x - p1.x - INPUT_FIELD_WIDTH)/2, p1.y + (p2.y - p1.y)/3 + BUTTON_HEIGHT};

    Point p_f_p1 = {p1.x + (p2.x - p1.x - INPUT_FIELD_WIDTH)/2, p1.y + (p2.y - p1.y)/3 + BUTTON_HEIGHT + 20};
    Point p_f_p2 = {p2.x - (p2.x - p1.x - INPUT_FIELD_WIDTH)/2, p1.y + (p2.y - p1.y)/3 + 2*BUTTON_HEIGHT + 20};

    Button label(sg, {0,0}, {640,90},"Username login", rgba(0,0,0,255), rgba(255,255,255,255), Font22x40);

    Button username_field(sg, u_f_p1, u_f_p2, "Username", rgb(90,90,90), rgb(198,198,198), Font10x14);
    Button password_field(sg, p_f_p1, p_f_p2, "Password", rgb(90,90,90), rgb(198,198,198), Font10x14);
    Button login_button(sg, 
            {p2.x - (p2.x - p1.x - 2*BUTTON_WIDTH)/3 - BUTTON_WIDTH, p2.y - 3*BUTTON_HEIGHT/2}, 
            {p2.x - (p2.x - p1.x - 2*BUTTON_WIDTH)/3, p2.y - BUTTON_HEIGHT/2},
            "Login", rgb(20,20,20), rgb(230,0,0), Font16x27);
    Button facial_login_button(sg,
            {p1.x + (p2.x - p1.x - 2*BUTTON_WIDTH)/3, p2.y - 3*BUTTON_HEIGHT/2},
            {p1.x + (p2.x - p1.x - 2*BUTTON_WIDTH)/3 + BUTTON_WIDTH, p2.y - BUTTON_HEIGHT/2},
            "Facial Login", rgb(20,20,20), rgb(198,198,198), Font16x27);
    Button username_update(sg, u_f_p1, u_f_p2, username_input, rgb(90,90,90), rgb(198,198,198), Font10x14);
    Button password_update(sg, p_f_p1, p_f_p2, password_display, rgb(90,90,90), rgb(198,198,198), Font10x14);

    int i = 0;
    //  update input fields with wand click
    sc.setTypingCB([ &password_update, &username_update, &input_field_chosen, &username_input, &password_input, &password_display, &i](char c){

        // username field is chosen
        if(!input_field_chosen){
            username_input += c;
            username_update.setString(username_input);
            username_update.draw();
        }
        // password field is chosen
        else{
            if(password_input.length()==0){
                password_display += c;
            }
            else if(password_input.length()>0){
                password_display = "";
                for(i = 0; i < (int)password_input.length(); i++){
                    password_display += "*";
                }
                password_display += c;
            }
            password_input += c;

            password_update.setString(password_display);
            password_update.draw();
            
        }
    });

    sc.setGestureCB([&i, &password_update, &username_update, &input_field_chosen, &username_input, &password_input, &password_display](std::string gesture){

        if(!input_field_chosen && username_input.length() > 0 && gesture == "Delete"){
            username_input.pop_back();
            username_update.setString(username_input);
            username_update.draw();
        }

        else if (input_field_chosen && password_input.length() > 0 && gesture == "Delete"){
            password_input.pop_back();
            password_display = "";

            for(i=0; i < (int)password_input.length(); i++){

                password_display += "*";
            }

            password_update.setString(password_display);
            password_update.draw();
        }
    });

    bool login_begun = false;

    login_button.onTouch([&login_begun, &sc, &wifi, &username_input, &password_input, &login_button](Point p){
        if (!login_begun) {
            login_begun = true;
            wifi.SendUsername(username_input, password_input);

            username_input = "";
            password_input = "";

            sc.remDrawable(&login_button);
            if(wifi.ReadResponse()){
                std::cout << "login successfully" << std::endl;
                wifi.LoadGestureMapping();
                sc.stop(to_Home);
            } else {
                // else clear username and password input fields
                std::cout << "login fail" << std::endl;
                sc.stop(to_UsernameLogin);
            }
        }
    });

    facial_login_button.onTouch([&sc, &username_input, &password_input](Point p){

        username_input = "";
        password_input = "";
        sc.stop(to_FacialLogin);
    });


    username_field.onTouch([&input_field_chosen](Point p){
        std::cout << "Input Field Not Chosen" << std::endl;
        input_field_chosen = false;
    });
    password_field.onTouch([&input_field_chosen](Point p){
        std::cout << "Input Field Chosen" << std::endl;
        input_field_chosen = true;
    });

    username_update.onTouch([&input_field_chosen](Point p){
        std::cout << "Input Field Not Chosen" << std::endl;
        input_field_chosen = false;
    });
    password_update.onTouch([&input_field_chosen](Point p){
        std::cout << "Input Field Chosen" << std::endl;
        input_field_chosen = true;
    });

    sc.addDrawable(&login_button);
    sc.addDrawable(&facial_login_button);
    sc.addDrawable(&password_field);
    sc.addDrawable(&username_field);
    sc.addDrawable(&label);

    sc.addDrawable(&username_update);
    sc.addTouchable(&username_update);
    sc.addDrawable(&password_update);
    sc.addTouchable(&password_update);


    sc.addTouchable(&login_button);
    sc.addTouchable(&facial_login_button);
    sc.addTouchable(&password_field);
    sc.addTouchable(&username_field);

    sc.draw();
    sc.start();

    int ret = sc.run();

    nios.stop();

    return ret;
}

int showFacialLoginPanel(SimpleGraphics &sg, TouchControl &tc, GeometricRecognizer &geo, Wand &wand, NIOS_Processor &nios, Cursor &gesture_cursor, Cursor &typing_cursor, Cursor &mouse_cursor, Wifi &wifi, Video &video){


    std::cout << "Facial Login Screen Running" << std::endl;
    Screen sc(sg, tc, geo, wand, nios, gesture_cursor, typing_cursor, mouse_cursor);

    static constexpr int BUTTON_HEIGHT = 50;
    static constexpr int PICTURE_HEIGHT = 120;
    static constexpr int PICTURE_WIDTH = 160;

    Point p1(0,0);
    Point p2(640,480);

    Button label(sg, {0,0}, {640,90},"Facial login", rgba(0,0,0,255), rgba(255,255,255,255), Font22x40);
    Button picture_boarder(sg, {240-2,170-2}, {240+PICTURE_WIDTH+2, 170+PICTURE_HEIGHT+2},
            "", rgb(20,20,20), rgba(0,0,0,255), Font16x27);
    Button picture_field(sg, {240,170}, {240+PICTURE_WIDTH, 170+PICTURE_HEIGHT},
            "", rgb(20,20,20), rgba(198,198,198,0), Font16x27);
    Button login_button(sg, {420, 440 - BUTTON_HEIGHT}, {520, 440},
            "Login", rgb(20,20,20), rgb(230,0,0), Font16x27);
    Button Un_Pwd_login_button(sg, {120, 440 - BUTTON_HEIGHT},  {380, 440},
            "Username Login", rgb(20,20,20), rgb(198,198,198), Font16x27);
    Button exit_button(sg, {540, 440 - BUTTON_HEIGHT}, {620, 440},
            "Exit", rgb(20,20,20), rgb(230,0,0), Font16x27);

    bool login_begun = false;
    
    login_button.onTouch([&login_begun, &sc, &wifi, &video, &login_button](Point p){
        if (!login_begun) {
            login_begun = true;
            std::cout<<"start sending picture"<<std::endl;
            wifi.SendPicture(video.takeRawPicture(120, 85));
            std::cout<<"send finish"<<std::endl;

            sc.remDrawable(&login_button);
            if(wifi.ReadResponse()){
                std::cout << "login successfully" << std::endl;
                wifi.LoadGestureMapping();
                sc.stop(to_Home);
            } else {
                std::cout << "login fail" << std::endl;
                sc.stop(to_UsernameLogin);
            }
        }
    });

    Un_Pwd_login_button.onTouch([&sc](Point p){
        sc.stop(to_UsernameLogin);
    });

    exit_button.onTouch([&sc](Point p){
        sc.stop(to_Home);
    });

    sc.addDrawable(&label);
    sc.addDrawable(&picture_boarder);
    sc.addDrawable(&picture_field);
    sc.addDrawable(&login_button);
    sc.addTouchable(&login_button);
    sc.addDrawable(&Un_Pwd_login_button);
    sc.addTouchable(&Un_Pwd_login_button);
    sc.addDrawable(&exit_button);
    sc.addTouchable(&exit_button);
    sc.draw();
    sc.start();

    int ret = sc.run();

    nios.stop();

    return ret;
}

int main(void)
{

    // Simplegraphics
    SimpleGraphics sg(640, 480);
    sg.clear();

    // Touch
    TouchControl tc("/dev/ttyAL0", sg.width(), sg.height(), true);
    tc.touch_enable();

    GeometricRecognizer geo;
    for (char c = 'A'; c < 'Z'; c++) {
        char lc = c + 'a' - 'A';
        geo.loadCharData(c);
        geo.loadCharData(lc);
    }
    geo.loadSamples();

    // Bluetooth
    Wand wand("/dev/ttyAL3", B115K);

    // NIOS
    FIFO_Serial nios_serial(NIOS_HPS_FIFO_BASE, NIOS_HPS_FIFO_STATUS_BASE,
                            HPS_NIOS_FIFO_BASE, HPS_NIOS_FIFO_STATUS_BASE);
    NIOS_Processor nios(nios_serial);

    // Wifi
    Wifi wifi("/dev/ttyAL1");

    // Video
    Video video("/dev/ttyAL2", VIDEO_FRAMEBUFFER_BASE, VIDEO_FRAMEBUFFER_SPAN);
    video.imageSettings(0, 0x80, 0x80, 0, 130);
    video.mirror_mode_on();

    // Define the cursor bitmaps, we store them in a separate file
    SimpleGraphics::rgba_t gesture_bitmap[Cursor::size][Cursor::size] =
#include "red_arrow_cursor.txt"
    ;

    SimpleGraphics::rgba_t typing_bitmap[Cursor::size][Cursor::size] =
#include "typing_cursor.txt"
    ;

    SimpleGraphics::rgba_t mouse_bitmap[Cursor::size][Cursor::size] =
#include "black_arrow_cursor.txt"
    ;

    Cursor gesture_cursor(sg, gesture_bitmap);

    Cursor typing_cursor(sg, typing_bitmap);

    Cursor mouse_cursor(sg, mouse_bitmap);

    int nextScreen = to_FacialLogin;
    std::string username_input = "";

    showStartScreen(sg, tc, geo, wand, nios, gesture_cursor, typing_cursor, mouse_cursor, nios_serial);
    while (1)
    {
        switch (nextScreen){
            case to_Home:
            {
                nextScreen = showHomePage(sg, tc, geo, wand, nios, gesture_cursor, typing_cursor, mouse_cursor, wifi, username_input);
                break;
            }
            case to_Bluetooth:
            {
                nextScreen = showBluetoothPage(sg, tc, geo, wand, nios, gesture_cursor, typing_cursor, mouse_cursor);
                break;
            }
            case to_FacialLogin:
            {
                nextScreen = showFacialLoginPanel(sg, tc, geo, wand, nios, gesture_cursor, typing_cursor, mouse_cursor, wifi, video);
                break;
            }
            case to_UsernameLogin:
            {
                nextScreen = showLoginPanel(sg, tc, geo, wand, nios, gesture_cursor, typing_cursor, mouse_cursor, wifi, video, username_input);
                break;
            }
            case to_Setting:
            {
                nextScreen = showSetting(sg, tc, geo, wand, nios, gesture_cursor, typing_cursor, mouse_cursor, video);
                break;
            }

            default:
            assert(0);
        }
    }

    return 0;
}
