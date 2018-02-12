#include <functional>
#include <iostream>
#include <assert.h>
#include "system.h"
#include <sys/alt_alarm.h>
#include "sys/alt_timestamp.h"
#include "touch.hpp"
#include "io.h"
#include "SimpleGraphics.hpp"
#include "UI.hpp"
#include "screen.hpp"

#define DEBOUNCE_MS 100

#define SG_MAX_WIDTH 160
#define SG_MAX_HEIGHT 120

int main(void) {
    SimpleGraphics graphics(reinterpret_cast<SimpleGraphics::rgba_t *>(DRAW_BUFFER_BASE), 
            SG_MAX_WIDTH, SG_MAX_HEIGHT);

    TouchControl touch(TOUCHSCREEN_UART_NAME, TOUCHSCREEN_UART_IRQ, TOUCHSCREEN_UART_IRQ_INTERRUPT_CONTROLLER_ID,
            SG_MAX_WIDTH, SG_MAX_HEIGHT, true);

    Screen screen(graphics, touch);
    Rectangle rect(graphics, {0, 0}, {20, 40}, SimpleGraphics::rgba(255, 0, 0, 255));
    Button button(graphics, touch, {20, 0}, {40, 20}, "Hello", 
            SimpleGraphics::rgba(255, 255, 255, 255),
            SimpleGraphics::rgba(0, 0, 255, 255));

    DropdownMenu menu(graphics, touch, DropdownMenu::TOP, {50, 0}, {90, 20}, "Drop", 
            SimpleGraphics::rgba(255, 255, 255, 255),
            SimpleGraphics::rgba(255, 255, 0, 255));

    menu.newItem(graphics, touch, "My button", SimpleGraphics::rgba(255, 255, 255, 255),
            SimpleGraphics::rgba(0, 0, 255, 255),
            [] (Touchable *, Point p) {
            std::cout << "Dropdown menu button got touched" << std::endl;

            });


    button.onTouch([] (Touchable *, Point p) {
            std::cout << "Button Got Touched" << std::endl;

            });

    screen.addDrawable(&rect);
    screen.addDrawable(&button);
    screen.addTouchable(&button);
    screen.addDrawable(&menu);
    screen.addTouchable(&menu);

    std::cout << "Starting" << std::endl;

    graphics.draw_rect(graphics.rgba(0, 0, 0, 255), 0, 0, SG_MAX_WIDTH, SG_MAX_HEIGHT);

    screen.enable_touch();

    screen.draw();

    while (1) { 
        touch.poll();
    }
#if 0
    // Setting switch 0 will go into calibration mode
    if (IORD_8DIRECT(SWITCH_IN_PIO_BASE, 0) & 0x1) {
        unsigned cal_count = 0;
        volatile bool done = false;

        auto cb = [&cal_count, &done] (TouchControl *, TouchUart::message *msg) -> void {
            //assert(msg->TYPE == TouchUart::MESSAGE);
            //assert(msg->body.response.CMND == TouchUart::CALIBRATE);
            //assert(msg->body.response.STATUS = TouchUart::OK);
            std:: cout << "Hello" << std::endl;

            if (cal_count == 0) {
                TouchControl::print(msg);
            } else if (cal_count <= 4) {
                TouchControl::print(msg);
                std::cout << "Got Point #" << cal_count << std::endl;
            } else {
                done = true;
            }
        };

        touch.setMessageCB(cb);
        touch.touch_disable();
        touch.poll();
        touch.calibrate();
        while (!done) {
            touch.poll();
        }
        std::cout << "Done Calibration" << std::endl;
    }

    auto touchCB = [&graphics] (TouchControl *, unsigned x, unsigned y) -> void {
        std::cout << "TOUCH X: " << x << " Y: " << y << std::endl;;
        graphics.draw_x(SimpleGraphics::rgba(0, 255, 0, 255), x, y, 4);
    };

    touch.setMessageCB(TouchControl::printCB);
    touch.setTouchCB(touchCB);
    touch.touch_enable();
    std::cout << "Running" << std::endl;

    while (1) { 
        touch.poll();
    }
#endif

    return 0;
}
