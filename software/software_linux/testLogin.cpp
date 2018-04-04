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

    FIFO_Serial nios_serial(NIOS_HPS_FIFO_BASE, NIOS_HPS_FIFO_STATUS_BASE,
            HPS_NIOS_FIFO_BASE, HPS_NIOS_FIFO_STATUS_BASE);

    NIOS_Processor nios(nios_serial);

    nios.hello();

    SimpleGraphics sg(640, 480);
    sg.clear();

    TouchControl tc("/dev/ttyAL0", 0, 0, 640, 480, true);
    tc.touch_enable();

    Wifi wifi("/dev/ttyAL1");

    Video video("/dev/ttyAL2");

    Wand wand("/dev/ttyAL3", B115K);

    WandControl wc(wand, nios);

	Path2D newPath;

    LoginPanel lp(sg, wifi, video, wc, {0,0}, {640,480}, Font16x27, Font10x14 );

    // user click input fields they want to write to 
    // between 2 wand clicks, updateLoginFields()
    // login when button is clicked
    lp.draw();

    lp.run();

    return 0;
}