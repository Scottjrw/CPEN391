#ifndef SIMPLE_GRAPHICS_HPP
#define SIMPLE_GRAPHICS_HPP

#include <stdint.h>
#include <stdio.h>
#include "system.h"
#include "io.h"
    
class SimpleGraphics {
public:
    typedef uint16_t rgba_t;
    
    inline rgba_t rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
        const unsigned int pixel_bits = 16;
        return (convert_8bit(b, pixel_bits))
            | (convert_8bit(g, pixel_bits) << pixel_bits / 4)
            | (convert_8bit(r, pixel_bits) << (2 * pixel_bits / 4))
            | (convert_8bit(a, pixel_bits) << (3 * pixel_bits / 4));
    }

    inline void draw_pixel(rgba_t color, unsigned int x, unsigned int y) {
        rgba_t *addr = m_buffer_base + (y * m_width + x);

        // Ignore invalid writes
        if (addr < m_max_addr)
            IOWR_16DIRECT(addr, 0, color);
    }

    void draw_rect(rgba_t color, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
        for (unsigned int i = x1; i < x2; i++)
            for (unsigned int j = y1; j < y2; j++)
                draw_pixel(color, i, j);
    }

    void draw_x(rgba_t color, unsigned int x, unsigned int y, int radius) {
        for (int i = -radius; i <= radius; i++) {
            for (int j = -radius; j <= radius; j++) {
                if (i == j || -i == j) {
                    draw_pixel(color, x + i, y + j);
                }
            }
        }
    }

    void draw_text(unsigned x, unsigned y, rgba_t fontcolour, rgba_t backgroundcolour, int c, int Erase){
        unsigned row, column, theX = x, theY = y ;
        int pixels ;
        char theColour = fontcolour  ;
        int BitMask, theC = c ;

    // if x,y coord off edge of screen don't bother

        if ((x > m_width-1) || (y > m_height-1))
            return ;


        if (((short)(theC) >= (short)(' ')) && ((short)(theC) <= (short)('~'))) {
            theC = theC - 0x20 ;
            for(row = 0; (char)(row) < (char)(7); row ++)   {

                pixels = Font5x7[theC][row] ;
                BitMask = 16 ;

                for(column = 0; (char)(column) < (char)(5); column ++)  {

    // if a pixel in the character display it
                    if((pixels & BitMask))
                        draw_converted_pixel(theColour，theX+column, theY+row) ;

                    else {
                        if(Erase == TRUE)

    // if pixel is part of background (not part of character)
    // erase the background to value of variable BackGroundColour

                            draw_converted_pixel(backgroundcolour，theX+column, theY+row) ;
                    }
                    BitMask = BitMask >> 1 ;
                }
            }
        }


    }

SimpleGraphics(rgba_t *buffer_base, unsigned int width, unsigned int height) :
        m_buffer_base(buffer_base),
        m_max_addr(buffer_base + width * height),
        m_width(width),
        m_height(height)
    { }

private:
    rgba_t *const m_buffer_base;
    rgba_t *const m_max_addr;
    const unsigned int m_width, m_height;

    static constexpr uint32_t convert_8bit(uint8_t val, unsigned int pixel_bits) {
        return ((val >> (8 - pixel_bits / 4)) & ((1 << pixel_bits / 4) - 1));
    }
};


#endif
