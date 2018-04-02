#include <iostream>
#include <unistd.h>
#include <system_error>
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

using namespace UI;
using namespace DollarRecognizer;


int main(void) {
    SimpleGraphics sg(640, 480);
    sg.clear();

    FIFO_Serial nios_serial(NIOS_HPS_FIFO_BASE, NIOS_HPS_FIFO_STATUS_BASE,
            HPS_NIOS_FIFO_BASE, HPS_NIOS_FIFO_STATUS_BASE);

    NIOS_Processor nios(nios_serial);

    int err;
    if ((err = NIOS_Processor_Init::program_nios(SDRAM_FILE, 
                    SDRAM_BASE, SDRAM_SPAN, MM_RESET_BASE, MM_RESET_SPAN))) {

        std::cerr << "Failed to program NIOS Processor" << std::endl;
        return err;
    }

    std::clog << "NIOS Processor Programmed" << std::endl;
    usleep(500000);

    std::clog << "Saying hello to the NIOS Processor..." << std::endl;
    nios_serial.clear();
    if (nios.hello(1000)) {
        std::clog << "NIOS not responding... giving up" << std::endl;

        return 1;
    }

    std::cout << "Received a Hello from the NIOS" << std::endl;

    TouchControl tc("/dev/ttyAL0", sg.width(), sg.height(), true);
    tc.touch_enable();

    Button startBtn(sg, {20,20}, {220, 120}, "Start", rgb(0,0,0), rgb(119,119,119));
    Button endBtn(sg, {20,200}, {220, 300}, "End", rgb(0,0,0), rgb(119,119,119));

    GeometricRecognizer gr;

	gr.loadSamples();

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

    startBtn.onTouch([&nios](Point point){
        nios.start();
    });
    
    endBtn.onTouch([&nios, &newPath, &gr](Point point){
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
    sc.add(CloseHandler::make_exit_trypoll(0));

    sc.draw();

    sc.run();

    std::clog << "Stopping... " << std::endl;

    return 0;
}
