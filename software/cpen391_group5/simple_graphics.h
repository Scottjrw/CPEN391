#ifndef SIMPLE_GRAPHICS_H
#define SIMPLE_GRAPHICS_H

#include <stdint.h>
#include <stdio.h>
#include "system.h"
#include "io.h"

#define GRAPHICS_BASE ((uint32_t *) DRAW_BUFFER_BASE)

#define SG_MAX_WIDTH 160
#define SG_MAX_HEIGHT 120

static inline uint32_t sg_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return b | (g << 8) | (r << 16) | (a << 24);
}

static inline void sg_set_pixel(unsigned int x, unsigned int y, uint32_t rgba) {
    uint32_t *addr = GRAPHICS_BASE + (y * SG_MAX_WIDTH + x);

    IOWR_32DIRECT(addr, 0, rgba);
}

static inline void sg_clear_screen() {

    for (int i = 0; i < SG_MAX_WIDTH; i++)
        for (int j = 0; j < SG_MAX_HEIGHT; j++)
            sg_set_pixel(i, j, sg_rgba(0, 0, 0, 0));
}

static inline void sg_draw_x(unsigned int x, unsigned int y, int size, uint32_t rgba) {
    for (int i = -size; i <= size; i++) {
        for (int j = -size; j <= size; j++) {
            if (i == j || -i == j) {
                sg_set_pixel(x + i, y + j, rgba);
            }
        }
    }
}

#endif
