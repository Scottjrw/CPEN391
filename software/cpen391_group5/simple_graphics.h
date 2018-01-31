#ifndef SIMPLE_GRAPHICS_H
#define SIMPLE_GRAPHICS_H

#include <stdint.h>
#include <stdio.h>
#include "system.h"
#include "io.h"


#define SG_MAX_WIDTH 160
#define SG_MAX_HEIGHT 120
// Bits per pixel
#define SG_PIXEL_BITS 16

// Writes to graphics should be in the range [GRAPHICS_BASE, GRAPHICS_MAX) (not including MAX)
#define GRAPHICS_BASE ((uint16_t *) DRAW_BUFFER_BASE)
#define GRAPHICS_MAX (GRAPHICS_BASE + SG_MAX_HEIGHT * SG_MAX_WIDTH)

// Convert from a coordinate in range [0, xy_max] to graphics coordinate
#define SG_X_COORD(x, x_max) ((x * SG_MAX_WIDTH) / x_max)
#define SG_Y_COORD(y, y_max) ((y * SG_MAX_HEIGHT) / y_max)

#define _SG_COLOR_BITS (SG_PIXEL_BITS / 4)
#define _SG_SHIFT_BITS (8 - _SG_COLOR_BITS)
#define _SG_SHIFT_MASK ((1 << _SG_COLOR_BITS) - 1)
#define _SG_CONV_8BIT(val) ((val >> _SG_SHIFT_BITS) & _SG_SHIFT_MASK)

typedef uint16_t sg_rgba_t;

static inline sg_rgba_t sg_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return (_SG_CONV_8BIT(b))
        | (_SG_CONV_8BIT(g) << _SG_COLOR_BITS)
        | (_SG_CONV_8BIT(r) << (2 * _SG_COLOR_BITS))
        | (_SG_CONV_8BIT(a) << (3 * _SG_COLOR_BITS));
}

static inline void sg_set_pixel(unsigned int x, unsigned int y, sg_rgba_t rgba) {
    sg_rgba_t *addr = GRAPHICS_BASE + (y * SG_MAX_WIDTH + x);

    // Ignore invalid writes
    if (addr < GRAPHICS_MAX)
        IOWR_16DIRECT(addr, 0, rgba);
}

static inline void sg_clear_screen() {
    for (int i = 0; i < SG_MAX_WIDTH; i++)
        for (int j = 0; j < SG_MAX_HEIGHT; j++)
            sg_set_pixel(i, j, sg_rgba(0, 0, 0, 0));
}

static inline void sg_draw_x(unsigned int x, unsigned int y, int size, sg_rgba_t rgba) {
    for (int i = -size; i <= size; i++) {
        for (int j = -size; j <= size; j++) {
            if (i == j || -i == j) {
                sg_set_pixel(x + i, y + j, rgba);
            }
        }
    }
}

#endif
