#include "SimpleGraphics.hpp"
#include <assert.h>
#include "Fonts.c"
#include <sys/alt_irq.h>

void SimpleGraphics::draw_rect(rgba_t color, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
    /*for (unsigned int i = x1; i < x2; i++)
        for (unsigned int j = y1; j < y2; j++)
            draw_pixel(color, i, j);*/

	CMD cmd = {x1, x2, y1, y2, color, DRAW_RECT};

	this->commands.push(cmd);

	if (!m_pending_irq)
		graphics_interrupt(this);

}

void SimpleGraphics::draw_x(rgba_t color, unsigned int x, unsigned int y, int radius) {
    for (int i = -radius; i <= radius; i++) {
        for (int j = -radius; j <= radius; j++) {
            if (i == j || -i == j) {
                draw_pixel(color, x + i, y + j);
            }
        }
    }
}

void SimpleGraphics::draw_char(rgba_t color, unsigned x, unsigned y, char c) {
    if (x >= m_width - 5 || y >= m_height - 7)
        return;

    if (c >= ' ' && c <= '~') {
        unsigned font_index = c - ' ';
        for (unsigned row = 0; row < 7; row ++) {
            unsigned pixels = Font5x7[font_index][row] ;
            unsigned bit = 0b10000;

            for (unsigned column = 0; column < 5; column++)  {
                if (pixels & bit)
                    draw_pixel(color, x+column, y+row) ;

                bit >>= 1;
            }
        }
    }
}

void SimpleGraphics::draw_string(rgba_t color, unsigned x, unsigned y, std::string str) {
    for (unsigned i = 0; i < str.length(); i++) {
        draw_char(color, x, y, str[i]);
        x += 5;
    }
}

void SimpleGraphics::startIRQ() {
    // Enable Interrupt on CPU
    alt_ic_isr_register(m_ic_id, m_irq_id, graphics_interrupt, this, nullptr);

}

void SimpleGraphics::graphics_interrupt(void* graphicsctrl){
	SimpleGraphics * graphics = static_cast<SimpleGraphics *> (graphicsctrl);

    std::cout << "Interrupt" << std::endl;

	if (!graphics->commands.empty()) {
        CMD cmd = graphics->commands.front();
        graphics->commands.pop();

        IOWR_32DIRECT(graphics->m_graphics_base, X1, cmd.X1);
        IOWR_32DIRECT(graphics->m_graphics_base, X2, cmd.X2);
        IOWR_32DIRECT(graphics->m_graphics_base, Y1, cmd.Y1);
        IOWR_32DIRECT(graphics->m_graphics_base, Y2, cmd.Y2);
        IOWR_32DIRECT(graphics->m_graphics_base, COLOUR, cmd.COLOR);
        IOWR_32DIRECT(graphics->m_graphics_base, COMMAND, cmd.COMMAND);

        graphics->m_pending_irq = true;
    } else {
        IOWR_32DIRECT(graphics->m_graphics_base, X1, 0);
        IOWR_32DIRECT(graphics->m_graphics_base, X2, 0);
        IOWR_32DIRECT(graphics->m_graphics_base, Y1, 0);
        IOWR_32DIRECT(graphics->m_graphics_base, Y2, 0);
        IOWR_32DIRECT(graphics->m_graphics_base, COLOUR, 0);
        IOWR_32DIRECT(graphics->m_graphics_base, COMMAND, DRAW_RECT);

        graphics->m_pending_irq = false;
    }

    IOWR_32DIRECT(graphics->m_graphics_base, IRQ, 1);
}
