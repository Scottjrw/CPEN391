#include <iostream>
#include <unistd.h>
#include "fifo_serial.hpp"
#include "NIOS_Processor.hpp"
#include "arm_system.h"
#include "GeometricRecognizer.h"
#include "GeometricRecognizerTypes.h"
#include "UI.hpp"
#include "touch.hpp"

using namespace UI;
using namespace DollarRecognizer;

int main(void) {
    FIFO_Serial nios_serial(NIOS_HPS_FIFO_BASE, NIOS_HPS_FIFO_STATUS_BASE,
            HPS_NIOS_FIFO_BASE, HPS_NIOS_FIFO_STATUS_BASE);

    nios_serial.clear();

    NIOS_Processor nios(nios_serial);

    SimpleGraphics sg(640, 480);
    sg.clear();

    TouchControl tc("/dev/ttyAL0", 0, 0, 640, 480, true);
    tc.touch_enable();

    Button startBtn(sg, tc, {20,20}, {120, 50}, "Start", sg.rgba(0,0,0,255), sg.rgba(119,119,119,255));
    Button endBtn(sg, tc, {20,60}, {120, 90}, "End", sg.rgba(0,0,0,255), sg.rgba(119,119,119,255));
    startBtn.draw();
    endBtn.draw();

    tc.setTouchCB([&startBtn, &endBtn](TouchControl * touch_control, unsigned int x, unsigned int y){
        startBtn.touch({x,y});
        endBtn.touch({x,y});
    });


    GeometricRecognizer *gr = new GeometricRecognizer();

	gr->loadTemplates();

	Path2D newPath;

    nios.dot_location_cb([&newPath](unsigned x, unsigned y){

        newPath.push_back(Point2D(x,y));
    });

    startBtn.onTouch([&nios](Touchable * tc, Point point){
        nios.start();
    });
    
    endBtn.onTouch([&gr, &newPath, &nios](Touchable * tc, Point point){
        nios.stop();
        RecognitionResult result = gr->recognize(newPath);
        cout << "the gesture input is: " << result.name << "\n";
    });

    while(true){
        tc.trypoll();
        nios.trypoll();
    }

    return 0;
}
