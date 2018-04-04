
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

enum Switch_Page_Commands{
    to_Home = 0,
    to_GestureMap = 2,
    to_NewGesture = 3,
    to_Setting = 4,
    to_LoginPanel = 5
};

void showStartScreen(SimpleGraphics &sg, TouchControl &tc, NIOS_Processor &nios) {
    Screen screen(sg, tc);

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

    NIOS_Processor_Init init(SDRAM_FILE, SDRAM_BASE, SDRAM_SPAN, 
            MM_RESET_BASE, MM_RESET_SPAN, doneCB, progressCB);

    init.run(screen);

    screen.run();
}


/*
 * @Param:
 *      username:  callback from login() is login successfully
 *      mistery class reference: implement on Tuesday
 */
void showLoginPanel(SimpleGraphics &sg, Wifi &wifi, Wand &wand, GeometricRecognizer &gr, Video &video, NIOS_Processor &nios){

    // lp is a subclass of screen
    LoginPanel lp(sg, wifi, wand, gr, {0,0}, {640,480}, nios, video);

    lp.run();
}

void showGestureRecognition(SimpleGraphics &sg, GeometricRecognizer &gr, NIOS_Processor &nios){
    
    Screen sc;

    Wand wand("/dev/ttyAL3", B115K);

    Modes wandMode;

    WandCommands cmds;

    wand.setMode([&wandMode](Mode m){
        wandMode = m;
    });

    wand.setGesture([&cmds](WandCommands wc){
        cmds = wc;
    });

    DropdownMenu Menu(sg, {40,40}, {240,100}, "Menu", rgba(0,0,0,255), rgba(159,159,159,255));

    homeMenu.newItem(sg, "Home", rgba(80,80,80,255), rgba(180,180,180,255), [&sc](Point p){
        sc.stop(0);
    })

    homeMenu.newItem(sg, "New Gesture", rgba(80,80,80,255), rgba(180,180,180,255), [&sc](Point p){
        sc.stop(3);
    });

    homeMenu.newItem(sg, "Log Out", rgba(80,80,80,255), rgba(180,180,180,255),  [&sc](Point p){
        sc.stop(5);
    });

    //Button startBtn(sg, {320,380}, {420, 450}, "Start", rgb(0,0,0), rgb(119,119,119));
    //Button endBtn(sg, {460,380}, {560, 450}, "End", rgb(0,0,0), rgb(119,119,119));

	Path2D newPath;

    Point last_min, last_max;

    nios.dot_location_cb([&last_min, &last_max, &sg, &newPath](auto dot){
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

    if(wandMode == gestureMode && cmds == wandStart){}
        nios.start();
    }

    else if(wandMode == gestureMode && cmds == wandStop){
        std::cout << "Number of points: " << newPath.size() << std::endl;
        nios.stop();        
                                                                    //  #############################################  //
        RecognitionResult result = gr.recognize(newPath);           //  this part will be replaced with mistery class  //
                                                                    //  #############################################  //
        std::cout << "the gesture input is: " << result.name << std::endl;
    }

    std::cout << "Starting..." << std::endl;

    while(true){
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

int showHomePage(SimpleGraphics &sg, std::string username){

    Screen sc;
    
    DropdownMenu homeMenu(sg, {40,40}, {240,100}, "Menu", rgba(0,0,0,255), rgba(159,159,159,255));
    Button welcomeField(sg, {340, 40}, {600, 80}, "Welcome!" + username, rgba(255,255,255,255), rgba(159,159,159,255));
    
    homeMenu.newItem(sg, "Gesture Map", rgba(80,80,80,255), rgba(180,180,180,255), [&sc](Point p){
        sc.stop(2);
    })

    homeMenu.newItem(sg, "New Gesture", rgba(80,80,80,255), rgba(180,180,180,255), [&sc](Point p){
        sc.stop(3);
    });

    homeMenu.newItem(sg, "Settings", rgba(80,80,80,255), rgba(180,180,180,255), [&sc](Point p){
        sc.stop(4);
    });

    homeMenu.newItem(sg, "Log Out", rgba(80,80,80,255), rgba(180,180,180,255),  [&sc](Point p){
        sc.stop(5);
    });

    sc.addDrawable(&homeMenu);
    sc.addDrawable(&welcomeField);
    sc.addTouchable(&homeMenu);
    sc.addTouchable(&welcomeField);

    sc.draw();

    return sc.run();
}

int main(void) {

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


    while(1){
        switch(scret):{
            case to_Home:{
                scret = showHomePage(sg, "group 5");
                break;
            } 
            case to_LoginPanel:{
                scret = showLoginPanel();
                break;
            } 
            case to_GestureMap:{
                scret = showGestureRecognition(sg, gr, nios);
                break;
            } 
        }
    }

    return 0;
}
