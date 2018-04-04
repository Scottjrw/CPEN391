#include "SimpleGraphics.hpp"
#include "NIOS_Processor.hpp"
#include "NIOS_Processor_Init.hpp"
#include "screen.hpp"
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

    nios_init.printCB([&sg] (std::string str) {
        sg.draw_string_bg_centered(rgb(255, 255, 255), sg.width() / 2, 400, str, Font16x27);
        sg.draw_string_centered(rgb(0, 0, 255), sg.width() / 2, 400, str, Font16x27);
    });

    nios_init.progressCB([&sg] (unsigned progress) {
        sg.draw_rect(rgb(0, 0, 0), {220, 300}, {420, 340});
        sg.draw_rect(rgb(255, 0, 0), {220, 300}, {2 * progress + 220, 340});
    });

    nios_init.doneCB([&nios, &loadingScreen] () {
        if (nios.hello(100)) {
            std::clog << "Failed to instantiate NIOS" << std::endl;
            assert(0);
        }

        loadingScreen.stop(0);
    });

    loadingScreen.add(&nios, &NIOS_Processor::trypoll);
    loadingScreen.add(&tc, &TouchControl::trypoll);

    loadingScreen.run();

    return 0;
}
