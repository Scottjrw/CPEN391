#include "SimpleGraphics.hpp"
#include "event_loop.hpp"
#include "Animation.hpp"

using namespace UI;
using namespace SimpleGraphicsFonts;
using std::placeholders::_1;
using std::placeholders::_2;

int main(void) {
    SimpleGraphics sg(640, 480);
    sg.clear();

    Event_Loop ev;


    Animation<> fade_in(0, 255, 5);
    Animation<> fade_out(255, 0, -5);

    fade_out.cb([&sg, &ev, &fade_in] (int val, bool end) {
        sg.draw_string(rgba(255, 255, 0, val), 100, 100, "Animated String...", Font38x59);

        if (end) {
            std::cout << "Fade Out Done" << std::endl;
            fade_in.run(ev);
        }
    });

    fade_in.cb([&sg, &ev, &fade_out] (int val, bool end) {
        sg.draw_string(rgba(255, 255, 0, val), 100, 100, "Animated String...", Font38x59);

        if (end) {
            std::cout << "Fade In Done" << std::endl;
            fade_out.run(ev);
        }
    });

    fade_in.run(ev);
    ev.run();
}
