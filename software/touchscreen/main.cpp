#include <functional>
#include <fstream>
#include <iostream>
#include <assert.h>
#include "system.h"
#include <sys/alt_alarm.h>
#include <sys/alt_timestamp.h>
#include <unistd.h>
#include <fcntl.h>
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

        // Setting switch 0 will go into calibration mode
    if (IORD_8DIRECT(SWITCH_IN_PIO_BASE, 0) & 0x1) {
        std::cout << "Calibrating" << std::endl;
        constexpr unsigned FROM_EDGE_X = 18;
        constexpr unsigned FROM_EDGE_Y = 22;
        constexpr unsigned FROM_EDGE_X_2 = SG_MAX_WIDTH - FROM_EDGE_X;
        constexpr unsigned FROM_EDGE_Y_2 = SG_MAX_HEIGHT - FROM_EDGE_X;
        graphics.draw_rect(SimpleGraphics::rgba(0, 0, 0, 255), 0, 0, SG_MAX_WIDTH, SG_MAX_HEIGHT);
        graphics.draw_x(SimpleGraphics::rgba(255, 255, 255 ,255), FROM_EDGE_X, FROM_EDGE_Y, 2);
        graphics.draw_char(SimpleGraphics::rgba(255, 255, 255 ,255), FROM_EDGE_X + 2, FROM_EDGE_Y + 2, '1');
        graphics.draw_x(SimpleGraphics::rgba(255, 255, 255 ,255), FROM_EDGE_X_2, FROM_EDGE_Y, 2);
        graphics.draw_char(SimpleGraphics::rgba(255, 255, 255 ,255), FROM_EDGE_X_2 + 2, FROM_EDGE_Y + 2, '2');
        graphics.draw_x(SimpleGraphics::rgba(255, 255, 255 ,255), FROM_EDGE_X, FROM_EDGE_Y_2, 2);
        graphics.draw_char(SimpleGraphics::rgba(255, 255, 255 ,255), FROM_EDGE_X + 2, FROM_EDGE_Y_2 + 2, '3');
        graphics.draw_x(SimpleGraphics::rgba(255, 255, 255 ,255), FROM_EDGE_X_2, FROM_EDGE_Y_2, 2);
        graphics.draw_char(SimpleGraphics::rgba(255, 255, 255 ,255), FROM_EDGE_X_2 + 2, FROM_EDGE_Y_2 + 2, '4');
        unsigned cal_count = 1;
        volatile bool done = false;

        auto cb = [&cal_count, &done] (TouchControl *, TouchUart::message *msg) -> void {
            //assert(msg->TYPE == TouchUart::MESSAGE);
            //assert(msg->body.response.CMND == TouchUart::CALIBRATE);
            //assert(msg->body.response.STATUS = TouchUart::OK);

            if (cal_count <= 4) {
                TouchControl::print(msg);
                std::cout << "Got Point #" << cal_count << std::endl;
            } else {
                done = true;
            }
        };

        touch.setMessageCB(touch.printCB);
        touch.touch_disable();
        touch.poll();

        touch.calibrate();
        touch.poll();

        touch.setMessageCB(cb);
        while (!done) {
            touch.poll();
        }
        std::cout << "Done Calibration" << std::endl;
    }
    
    graphics.draw_rect(graphics.rgba(0, 0, 0, 255), 0, 0, SG_MAX_WIDTH, SG_MAX_HEIGHT);

    screen.draw();

    screen.enable_touch();

    touch.touch_enable();

    std::cout << "Running" << std::endl;

    while (1) { 
        touch.trypoll();
    }

    return 0;
}
