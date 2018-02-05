#ifndef SIMPLE_GRAPHICS_H
#define SIMPLE_GRAPHICS_H

#include <stdint.h>
#include <stdio.h>
#include "system.h"
#include "io.h"

namespace SG_Pixel_Conversions {
    constexpr uint32_t convert_8bit(uint8_t val, unsigned int pixel_bits) {
        return ((val >> (8 - pixel_bits / 4)) & ((1 << pixel_bits / 4) - 1));
    }

    template <typename T>
    inline T rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

    template <>
    inline uint16_t rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
        const unsigned int pixel_bits = 16;
        return (convert_8bit(b, pixel_bits))
            | (convert_8bit(g, pixel_bits) << pixel_bits / 4)
            | (convert_8bit(r, pixel_bits) << (2 * pixel_bits / 4))
            | (convert_8bit(a, pixel_bits) << (3 * pixel_bits / 4));
    }

    template <>
    inline uint32_t rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
        const unsigned int pixel_bits = 32;
        return (convert_8bit(b, pixel_bits))
            | (convert_8bit(g, pixel_bits) << pixel_bits / 4)
            | (convert_8bit(r, pixel_bits) << (2 * pixel_bits / 4))
            | (convert_8bit(a, pixel_bits) << (3 * pixel_bits / 4));
    }
}

         // Type representing each pixel, for 16 bit RGBA use uint16_t, 32 bit RGBA use uint32_t
         // Note that if the wrong type is used, this won't compile
template <typename rgba_t,
         // Width and Height of the draw buffer
         unsigned int WIDTH, unsigned int HEIGHT, 
         // If input is from a different resolution do conversion
         unsigned int INPUT_WIDTH = WIDTH, unsigned int INPUT_HEIGHT = HEIGHT>
class SimpleGraphics {
    public:

    inline rgba_t rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
        return SG_Pixel_Conversions::rgba<rgba_t>(r, g, b, a);
    }

    inline void draw_pixel(unsigned int x, unsigned int y, rgba_t color) {
        convert_coord(x, y);
        draw_converted_pixel(x, y, color);
    }

    void fill(rgba_t color) {
        for (unsigned int i = 0; i < WIDTH; i++)
            for (unsigned int j = 0; j < HEIGHT; j++)
                draw_pixel(i, j, color);
    }

    void draw_x(unsigned int x, unsigned int y, int size, rgba_t color) {
        convert_coord(x, y);
        for (int i = -size; i <= size; i++) {
            for (int j = -size; j <= size; j++) {
                if (i == j || -i == j) {
                    draw_converted_pixel(x + i, y + j, color);
                }
            }
        }
    }

    SimpleGraphics(rgba_t *buffer_base) :
        m_buffer_base(buffer_base),
        m_max_addr(buffer_base + WIDTH * HEIGHT)
    { }

    private:
    rgba_t *const m_buffer_base;
    rgba_t *const m_max_addr;

    inline void convert_coord(unsigned int &x, unsigned int &y) {
        x = (x * WIDTH) / INPUT_WIDTH;
        y = (y * HEIGHT) / INPUT_HEIGHT;
    }

    inline void draw_converted_pixel(unsigned int x, unsigned int y, rgba_t color) {
        rgba_t *addr = m_buffer_base + (y * WIDTH + x);

        // Ignore invalid writes
        if (addr < m_max_addr)
            IOWR_16DIRECT(addr, 0, color);
    }
};


#endif
