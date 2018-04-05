#include "SimpleGraphics.hpp"

int main(void) {
    using namespace SimpleGraphicsFonts;

    SimpleGraphics sg(640, 480);
    sg.clear();
    sg.draw_rect(rgb(0, 0, 0), 0, 0, sg.width(), sg.height());


    sg.draw_string(rgb(0, 255, 0), {100, 100} , "Font5x7", Font5x7);
    sg.draw_string(rgb(0, 255, 0), {100, 110} , "Font10x14", Font10x14);
    sg.draw_string(rgb(0, 255, 0), {100, 130} , "Font16x27", Font16x27);
    sg.draw_string(rgb(0, 255, 0), {100, 160} , "Font22x40", Font22x40);
    sg.draw_string(rgb(0, 255, 0), {100, 210} , "Font38x59", Font38x59);

    while(1);
}
