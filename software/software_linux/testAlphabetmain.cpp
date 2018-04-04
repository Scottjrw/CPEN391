#include <iostream>
#include <unistd.h>
#include <fstream>
#include "arm_system.h"
#include "UI.hpp"
#include "touch.hpp"
#include "screen.hpp"
#include "GeometricRecognizer.hpp"

using namespace UI;
using namespace SimpleGraphicsFonts;
using namespace DollarRecognizer;

int main(void){

    SimpleGraphics sg(640, 480);
    sg.clear();

    TouchControl tc("/dev/ttyAL0", sg.width(), sg.height(), true);
    tc.touch_enable();

    Screen sc(sg, tc);

    Button startBtn(sg, {40,40}, {140, 100}, "Start", rgb(0,0,0), rgb(119,119,119), Font16x27);
    Button endBtn(sg, {40,140}, {140, 200}, "End", rgb(0,0,0), rgb(119,119,119), Font16x27);
    Button nextBtn(sg, {40,240}, {140, 300}, "Next", rgb(0,0,0), rgb(119,119,119), Font16x27);
    Button touchField(sg, {200, 40}, {600, 440}, "x", rgb(0,0,0), rgb(255,255,255), Font16x27);

    std::ofstream f;
    char c = 'A';
    std::string cur_filename = "A.txt";
    bool file_is_open = false;
    bool next_character = false;


    startBtn.onTouch([&touchField, &f, &cur_filename, &c, &file_is_open](Point point){
        if (!file_is_open) {
            std::cout << "Start drawing the character: " << c << std::endl;
            f.open(cur_filename.c_str(), std::ios_base::out);
            touchField.draw();
            file_is_open = true;
        }
    });
    
    endBtn.onTouch([&f, &file_is_open, &next_character](Point point){
        if (file_is_open) {
            file_is_open = false;
            std::cout << "Stop..."<< std::endl;
            f.close();
            next_character = true;
        }
    });

    nextBtn.onTouch([&c, &cur_filename, &next_character] (Point p){
        if (next_character) {
            if (c == 'Z')
                c = 'a';
            else if (c == 'z')
                c = 'A';
            else
                c++;

            cur_filename = c;
            cur_filename += ".txt";

            std::cout << "Character = " << c << std::endl;
            std::cout << "File = " << cur_filename << std::endl;
            next_character = false;
        }
    });

    touchField.onTouch([&f, &sg](Point point){
        std::cout << "( " << point.x << "," << point.y << ")" << std::endl;
        f << point.x << ',' << point.y << '\n';
        sg.draw_rect(rgba(255,0,0,255), {point.x, point.y}, {point.x+2, point.y+2});
    });

    sc.addTouchable(&startBtn);
    sc.addTouchable(&endBtn);
    sc.addTouchable(&touchField);
    sc.addTouchable(&nextBtn);
    sc.addDrawable(&startBtn);
    sc.addDrawable(&endBtn);
    sc.addDrawable(&touchField);
    sc.addDrawable(&nextBtn);

    sc.draw();

    return sc.run();
}
