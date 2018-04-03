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

    Bluetooth bt("/dev/ttyAL3");

    GeometricRecognizer gr;

	gr.loadSamples();

	Path2D newPath;

    nios.dot_location_cb([&newPath](unsigned x, unsigned y){
        std::cout << "Point: " << x << ',' << y << std::endl;
        newPath.push_back(Point(x,y));
    });

    LoginPanel lp(sg, wifi, bt, gr, {0,0}, {640,480}, nios, video);

    lp.draw();

    Screen sc(sg, tc);

    sc.addDrawable(&lp.m_username_field);
    sc.addDrawable(&lp.m_password_field);
    sc.addDrawable(&lp.m_login_button);
    sc.addDrawable(&lp.m_switch_login_mode_button);

    sc.addTouchable(&lp.m_username_field);
    sc.addTouchable(&lp.m_password_field);
    sc.addTouchable(&lp.m_login_button);
    sc.addTouchable(&lp.m_switch_login_mode_button);





}