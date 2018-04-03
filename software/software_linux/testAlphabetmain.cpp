#include <iostream>
#include <unistd.h>
#include "fifo_serial.hpp"
#include "NIOS_Processor.hpp"
#include "arm_system.h"
#include "GeometricRecognizer.hpp"
#include "GeometricRecognizerTypes.hpp"
#include "UI.hpp"
#include "touch.hpp"
#include "event_loop.hpp"
#include "screen.hpp"
#include "LoginPanel.hpp"

using namespace UI;
using namespace DollarRecognizer;

int main(void){

    screen sc;
    SimpleGraphics sg(640, 480);

    TouchControl tc("/dev/ttyAL0", sg.width(), sg.height(), true);
    tc.touch_enable();

    Button startBtn(sg, {40,40}, {140, 100}, "Start", rgb(0,0,0), rgb(119,119,119));
    Button endBtn(sg, {40,140}, {140, 200}, "End", rgb(0,0,0), rgb(119,119,119));
    Button touchField(sg, {200, 40}, {600, 440}, "", rgb(0,0,0), rgb(255,255,255));

    startBtn.onTouch([](Point point){
        std::cout << "Start drawing the character:"<< std::endl;
    });
    
    sc.onTouch([](Point point){
        std::print << "( " << point.x << "," << point.y << ")" << std::endl;
        sg.draw_rect(rgba(0,0,0,255), {point.x, point.y}, {point.x+2, point.y+2});
    });

    endBtn.onTouch(](Point point){
        std::cout << "Stop..."<< std::endl;
    });

    sc.addTouchable(&startBtn);
    sc.addTouchable(&endBtn);
    sc.addDrawable(&startBtn);
    sc.addDrawable(&endBtn);

    sc.draw();

    return sc.run();

    return 0;
}