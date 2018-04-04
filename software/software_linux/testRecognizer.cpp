#include "SimpleGraphics.hpp"
#include "touch.hpp"
#include "UI.hpp"
#include "NIOS_Processor.hpp"
#include "screen.hpp"
#include "GeometricRecognizer.hpp"

using namespace UI;
using namespace SimpleGraphicsFonts;
using namespace DollarRecognizer;

int main(void) {

    SimpleGraphics sg(640, 480);
    sg.clear();

    TouchControl tc("/dev/ttyAL0", sg.width(), sg.height(), true);
    tc.touch_enable();

    // NIOS
    FIFO_Serial nios_serial(NIOS_HPS_FIFO_BASE, NIOS_HPS_FIFO_STATUS_BASE,HPS_NIOS_FIFO_BASE, HPS_NIOS_FIFO_STATUS_BASE);

    NIOS_Processor nios(nios_serial);

     // Bluetooth
    Wand wand("/dev/ttyAL3", B1115K);


    WandControl wc(wand, nios);

    Screen sc(sg, tc, wc);

    Button btn(sg, {40,40}, {140, 100}, "Recognize!", rgb(0,0,0), rgb(119,119,119), Font16x27);
    Button touchField(sg, {200, 40}, {600, 440}, "x", rgb(0,0,0), rgb(255,255,255), Font16x27);

    GeometricRecognizer recognizer;

    for (char c = 'A'; c <= 'Z'; c++)
        recognizer.loadCharData(c);

    for (char c = 'a'; c <= 'z'; c++)
        recognizer.loadCharData(c);

    Path2D path;

    btn.onTouch([&touchField, &path, &recognizer](Point point){
        if (path.size() > 0) {
            auto result = recognizer.recognize(path);
            std::cout << "Result: " << result.name << " score: " << result.score << std::endl;
            touchField.draw();
            path.clear();
        }
    });

    touchField.onTouch([&sg, &path](Point point){
        std::cout << "( " << point.x << "," << point.y << ")" << std::endl;
        path.emplace_back(point.x, point.y);
        sg.draw_rect(rgba(255,0,0,255), {point.x, point.y}, {point.x+2, point.y+2});
    });

    sc.addTouchable(&touchField);
    sc.addDrawable(&touchField);
    sc.addTouchable(&btn);
    sc.addDrawable(&btn);

    sc.draw();

    sc.run();

    return 0;
}
