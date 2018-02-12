#include <functional>
#include <iostream>
#include <assert.h>
#include "system.h"
#include "sys/alt_alarm.h"
#include "sys/alt_timestamp.h"
#include "touch.hpp"
#include "io.h"
#include "SimpleGraphics.hpp"

#define DEBOUNCE_MS 100

#define SG_MAX_WIDTH 160
#define SG_MAX_HEIGHT 120


int main(void) {
    SimpleGraphics graphics(reinterpret_cast<SimpleGraphics::rgba_t *>(DRAW_BUFFER_BASE), 
            SG_MAX_WIDTH, SG_MAX_HEIGHT);

    TouchControl touch(TOUCHSCREEN_UART_NAME, TOUCHSCREEN_UART_IRQ, TOUCHSCREEN_UART_IRQ_INTERRUPT_CONTROLLER_ID,
            SG_MAX_WIDTH, SG_MAX_HEIGHT, true);

    std::cout << "Starting" << std::endl;

    graphics.draw_rect(graphics.rgba(0, 0, 0, 255), 0, 0, SG_MAX_WIDTH, SG_MAX_HEIGHT);

    // Setting switch 0 will go into calibration mode
    if (IORD_8DIRECT(SWITCH_IN_PIO_BASE, 0) & 0x1) {
        unsigned cal_count = 0;
        volatile bool done = false;

        auto cb = [&cal_count, &done] (TouchControl *, TouchUart::message *msg) -> void {
            assert(msg->TYPE == TouchUart::MESSAGE);
            assert(msg->body.response.CMND == TouchUart::CALIBRATE);
            assert(msg->body.response.STATUS = TouchUart::OK);

            if (cal_count == 0) {
                TouchControl::print(msg);
            } else if (cal_count <= 4) {
                std::cout << "Got Point #" << cal_count << std::endl;
            } else {
                done = true;
            }
        };

        touch.setMessageCB(cb);
        while (!done) {
            touch.poll();
        }
        std::cout << "Done Calibration" << std::endl;
    }

    auto touchCB = [] (TouchControl *, unsigned x, unsigned y) -> void {
        std::cout << "TOUCH X: " << x << " Y: " << y << std::endl;;
    };

    touch.setMessageCB(TouchControl::printCB);
    touch.setTouchCB(touchCB);
    touch.touch_enable();
    touch.startIRQ();
    std::cout << "Running" << std::endl;

    while (1) { 
        //touch.poll();
    }

    return 0;
}
