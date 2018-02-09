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

    void draw_rect(rgba_t color, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);

    void draw_x(rgba_t color, unsigned int x, unsigned int y, int radius);

    void draw_text(unsigned x, unsigned y, rgba_t fontcolour, rgba_t backgroundcolour, int c, int Erase);

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
