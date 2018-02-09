#include "SimpleGraphics.hpp"
#include "Fonts.c"

void SimpleGraphics::draw_rect(rgba_t color, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
    for (unsigned int i = x1; i < x2; i++)
        for (unsigned int j = y1; j < y2; j++)
            draw_pixel(color, i, j);
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
