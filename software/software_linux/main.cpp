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
#include "bluetooth.hpp"
#include "Animation.hpp"
//#include "LoginPanel.hpp"

using namespace UI;
using namespace DollarRecognizer;
using namespace SimpleGraphicsFonts;

// void addDropDownMenu(SimpleGraphics &sg, screen &sc, FontType menuFont){
//     DropdownMenu homeMenu(sg, {40, 40}, {240, 100}, "Menu", rgba(0, 0, 0, 255), rgba(159, 159, 159, 255), menuFont);
//     homeMenu.newItem(sg, "Home", rgba(80, 80, 80, 255), rgba(180, 180, 180, 255), [&sc](Point p) {
//                 sc.stop(0);
//     });

//     homeMenu.newItem(sg, "Gesture Map", rgba(80, 80, 80, 255), rgba(180, 180, 180, 255), [&sc](Point p) {
//                 sc.stop(2);
//     });

//     homeMenu.newItem(sg, "New Gesture", rgba(80, 80, 80, 255), rgba(180, 180, 180, 255), [&sc](Point p) {
//                 sc.stop(3);
//     });

//     homeMenu.newItem(sg, "Settings", rgba(80, 80, 80, 255), rgba(180, 180, 180, 255), [&sc](Point p) {
//                 sc.stop(4);
//     });

//     homeMenu.newItem(sg, "Log Out", rgba(80, 80, 80, 255), rgba(180, 180, 180, 255), [&sc](Point p) {
//                 sc.stop(5);
//     });

//     NIOS_Processor_Init init(SDRAM_FILE, SDRAM_BASE, SDRAM_SPAN, 
//             MM_RESET_BASE, MM_RESET_SPAN, doneCB, progressCB);

//     init.run(screen);

//     screen.run();
//     sc.addDrawable(homeMenu);
//     sc.addTouchable(homeMenu);
// }

// int showGestureRecognition(SimpleGraphics &sg, GeometricRecognizer &gr, Wifi &wifi, TouchControl &tc, Wand &wand, FontType menuFont)
// {

//     WandControl wc(wand, nios);
//     screen sc(sg, tc, wc);

//     addDropDownMenu(sg, sc, FontType menuFont);

//     Button startBtn(sg, {320,380}, {420, 450}, "Start", rgb(0,0,0), rgb(119,119,119));
//     Button endBtn(sg, {460,380}, {560, 450}, "End", rgb(0,0,0), rgb(119,119,119));

//     //Path2D newPath;

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

int showBluetoothPage(SimpleGraphics &sg, Wand &wand, WandControl &wc, TouchControl &tc){
    Screen sc(sg, tc, wc);
    
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
    
    fade_in.cb([&sg, &sc, &fade_in] (int val, bool end) {
        sg.draw_rect(rgba(0, 0, 0, 40), {10, 160}, {sg.width()-10, (unsigned)val});
        
        if (end) {
            std::cout << "Slide In Done" << std::endl;
        }
    });
    
    findDevices.onTouch([&available_devices, &wand, &sg, &fade_in, &sc, &selected_device_str, &device_buttons](Point p) {
        
        
        available_devices = wand.getAvailableDevices("7");
        
        fade_in.run(sc);
        
        // Available bluetooth devices
        for (int i = 0; i < (int)available_devices.size(); i++) {
            Button *b1 = new Button(sg, {10, 160+20*(unsigned)i}, {sg.width()/2, 180+20*(unsigned)i}, available_devices[i].first, rgba(0, 0, 0, 255), rgba(0, 0, 0, 40), Font10x14);

            Button *b2 = new Button(sg, {sg.width()/2, 160+20*(unsigned)i}, {sg.width()-10, 180+20*(unsigned)i}, available_devices[i].second, rgba(0, 0, 0, 255), rgba(0, 0, 0, 40), Font10x14);

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
        sc.stop(5);
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
    
    return sc.run();
}


int main(){
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
    // Bluetooth (wand)
    Wand wand("/dev/ttyAL3", B115K);
    WandControl wc(wand, nios);
    // Gesture Recognizer
    GeometricRecognizer gr;
    gr.loadSamples();

    while(1){
        showBluetoothPage(sg, wand, wc, tc);
    }

    return 0;
}