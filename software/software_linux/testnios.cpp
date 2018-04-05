#include "SimpleGraphics.hpp"
#include "NIOS_Processor.hpp"
#include "NIOS_Processor_Init.hpp"
#include "screen.hpp"
#include "video.hpp"
#include "arm_system.h"
#include "WandControl.hpp"
#include "UI.hpp"

using namespace SimpleGraphicsFonts;
using namespace UI;

int main(void) {

    // Simplegraphics
    SimpleGraphics sg(640, 480);
    sg.clear();

    // NIOS
    FIFO_Serial nios_serial(NIOS_HPS_FIFO_BASE, NIOS_HPS_FIFO_STATUS_BASE,
                            HPS_NIOS_FIFO_BASE, HPS_NIOS_FIFO_STATUS_BASE);

    TouchControl tc("/dev/ttyAL0", sg.width(), sg.height(), true);
    tc.touch_enable();

    NIOS_Processor nios(nios_serial);

    NIOS_Processor_Init nios_init(SDRAM_FILE, SDRAM_BASE, SDRAM_SPAN,
            MM_RESET_BASE, MM_RESET_SPAN);

    Wand wand("/dev/ttyAL3", B115K);

    WandControl wandctrl(wand, nios);

    Screen loadingScreen(sg, tc, wandctrl);

    Video video("/dev/ttyAL1", VIDEO_FRAMEBUFFER_BASE, VIDEO_FRAMEBUFFER_SPAN);

    loadingScreen.add_timer(std::chrono::seconds(1), [&sg, &nios_init] (Event_Loop *ev) -> bool {
        sg.fill(rgb(255, 255, 255));
        sg.draw_logo("logo.bmp", 120, 100);

        nios_init.run(*ev);

        return false;
    });

    nios_init.printCB([&sg] (std::string str) {
        sg.draw_string_bg_centered(rgb(255, 255, 255), sg.width() / 2, 400, 36, Font16x27);
        sg.draw_string_centered(rgb(0, 0, 255), sg.width() / 2, 400, str, Font16x27);
    });

    nios_init.progressCB([&sg] (unsigned progress) {
        std::cout << "Progress: " << progress << '%' << std::endl;
        sg.draw_rect(rgb(255, 0, 0), {220, 300}, {2 * progress + 220, 340});
        sg.draw_rect(rgb(0, 0, 0), {2*progress + 220, 300}, {420, 340});
    });

    nios_init.doneCB([&nios, &loadingScreen, &sg, &video] () {
        if (nios.hello(200)) {
            std::clog << "Failed to instantiate NIOS" << std::endl;
            assert(0);
        }

        std::clog << "Done Programming NIOS" << std::endl;

        sg.clear();

        loadingScreen.add_timer(std::chrono::milliseconds(500), [&sg, &video] (Event_Loop *) -> bool {
            uint8_t r, g, b;
            sg.draw_rect(rgba(255, 255, 255, 128), sg.width()/2 - 2, sg.height()/2 - 2, sg.width()/2 + 2, sg.height()/2 + 2);
            std::tie(r, g, b) = rgbReverse(video.getPixel(sg.width() / 4, sg.height() / 4));
            std::ostringstream text;
            text << "Pixel: " << static_cast<int>(r) << ',' << static_cast<int>(g) << ',' << static_cast<int>(b);
            sg.draw_string_bg(rgba(0, 0, 0, 0), 10, 10, 20, Font10x14);
            sg.draw_string(rgb(255, 255, 255), 10, 10, text.str(), Font10x14);

            return true;
        });
        std::clog << "Starting Points" << std::endl;
        nios.start();
        video.imageSettings(0, 0x80, 0x80, 0, 130);
        video.mirror_mode_on();
        std::clog << "Start Done" << std::endl;
    });

    int last_min_x = 0, last_min_y = 0, last_max_x = 1, last_max_y = 1;


    nios.dot_location_cb([&sg, &last_min_x, &last_min_y, &last_max_x, &last_max_y] (const NIOS_HPS_Protocol::Dot_Location::body &dot) {
        sg.draw_rect(rgba(0, 0, 0, 0), last_min_x, last_min_y, last_max_x, last_max_y);
        sg.draw_rect(rgba(0, 0, 255, 100), dot.min_x, dot.min_y, dot.max_x, dot.max_y);

        last_min_x = dot.min_x;
        last_min_y = dot.min_y;
        last_max_x = dot.max_x;
        last_max_y = dot.max_y;
    });

    loadingScreen.add(&nios, &NIOS_Processor::trypoll);
    loadingScreen.add(&tc, &TouchControl::trypoll);

    loadingScreen.draw();
    loadingScreen.run();

    return 0;
}
