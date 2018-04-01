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

using namespace UI;
using namespace DollarRecognizer;


int main(void) {
    FIFO_Serial nios_serial(NIOS_HPS_FIFO_BASE, NIOS_HPS_FIFO_STATUS_BASE,
            HPS_NIOS_FIFO_BASE, HPS_NIOS_FIFO_STATUS_BASE);

    NIOS_Processor nios(nios_serial);

    nios.hello();

    SimpleGraphics sg(640, 480);
    sg.clear();

    TouchControl tc("/dev/ttyAL0", 0, 0, 640, 480, true);
    tc.touch_enable();

    Button startBtn(sg, {20,20}, {220, 120}, "Start", rgb(0,0,0), rgb(119,119,119));
    Button endBtn(sg, {20,200}, {220, 300}, "End", rgb(0,0,0), rgb(119,119,119));

    GeometricRecognizer gr;

	gr.loadSamples();

	Path2D newPath;

    nios.dot_location_cb([&newPath](unsigned x, unsigned y){
        std::cout << "Point: " << x << ',' << y << std::endl;
        newPath.push_back(Point(x,y));
    });

    startBtn.onTouch([&nios](Point point){
        nios.start();
    });
    
    endBtn.onTouch([&gr, &newPath, &nios](Point point){
        std::cout << "Number of points: " << newPath.size() << std::endl;
        nios.stop();
        RecognitionResult result = gr.recognize(newPath);
        std::cout << "the gesture input is: " << result.name << std::endl;
    });

    std::cout << "Starting..." << std::endl;


    Screen sc(sg, tc);

    sc.addTouchable(&startBtn);
    sc.addTouchable(&endBtn);
    sc.addDrawable(&startBtn);
    sc.addDrawable(&endBtn);
    sc.add(&nios, &NIOS_Processor::trypoll);

    sc.run();


    return 0;
}
