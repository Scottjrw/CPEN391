#ifndef SIMPLE_GRAPHICS_HPP
#define SIMPLE_GRAPHICS_HPP

#include <cassert>
#include <type_traits>
#include <functional>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <cstdint>
#include <queue>
#include <stdio.h>
#include <sys/mman.h>
#include <stdint.h>
#include <errno.h>
#include <initializer_list>
#include "GeometricTypes.hpp"
#include "SimpleGraphicsFonts.hpp"

//#define HW_GRAPHICS

class SimpleGraphics {
public:
    using FontType = SimpleGraphicsFonts::FontType;
    typedef uint32_t rgba_t;

    inline void draw_pixel(rgba_t color, unsigned x, unsigned y) {

        volatile rgba_t *addr = m_buffer_base + (y * m_width + x);

        #ifdef HW_GRAPHICS
            while (!commands.empty()|| m_pending_irq) usleep(1);
        #endif

        // Ignore invalid writes
        if (m_buffer_base <= addr && addr < m_max_addr)
            *addr = color;
    }

    inline rgba_t read_pixel(unsigned x, unsigned y) {
		volatile rgba_t *addr = m_buffer_base + (y * m_width + x);

		// Ignore invalid writes
        if (m_buffer_base <= addr && addr < m_max_addr)
			return *addr;

		return 0;
	}

    inline void draw_rect(rgba_t color, Point p1, Point p2) {
        draw_rect(color, p1.x, p1.y, p2.x, p2.y);
    }

    void draw_rect(rgba_t color, unsigned x1, unsigned y1, unsigned x2, unsigned y2);

    inline void draw_x(rgba_t color, Point center, unsigned radius) {
        draw_x(color, center.x, center.y, radius);
    }

    void draw_x(rgba_t color, unsigned x, unsigned y, unsigned radius);

    inline void draw_char(rgba_t color, Point top_left, char c, FontType f) {
        draw_char(color, top_left.x, top_left.y, c, f);
    }

    void draw_char(rgba_t color, unsigned x, unsigned y, char c, FontType f);

    inline void draw_string(rgba_t color, Point top_left, std::string str, FontType f) {
        draw_string(color, top_left.x, top_left.y , str, f);
    }

    void draw_string(rgba_t color, unsigned x, unsigned y, std::string str, FontType f);

    void draw_string_centered(rgba_t color, Point center, std::string str, FontType f) {
        draw_string_centered(color, center.x, center.y, str, f);
    }

    void draw_string_centered(rgba_t color, unsigned x, unsigned y, std::string str, FontType f);

    void draw_string_bg_centered(rgba_t color, Point center, std::string str, FontType f) {
        draw_string_bg_centered(color, center.x, center.y, str, f);
    }

    void draw_string_bg_centered(rgba_t color, unsigned x, unsigned y, std::string str, FontType f);

    void clear();

    void fill(rgba_t color);

    void draw_logo(char * filename, unsigned x, unsigned y);

    void startIRQ();

    unsigned width() { return m_width; }
    unsigned height() { return m_height; }

 #ifdef HW_GRAPHICS
     SimpleGraphics(char *graphics_base, int ic_id, int irq_id, 
             rgba_t *buffer_base, unsigned int width, unsigned int height) :
         m_graphics_base(graphics_base),
         m_ic_id(ic_id),
         m_irq_id(irq_id),
         m_buffer_base(buffer_base),
         m_max_addr(buffer_base + width * height),
         m_width(width),
         m_height(height),
         m_pending_irq(false)
     { 
         assert(ic_id != -1);
         assert(irq_id != -1);
     }
 #else
     SimpleGraphics(unsigned int width, unsigned int height);
 #endif

private:

    rgba_t *m_buffer_base;
    rgba_t *m_max_addr;
    const unsigned int m_width, m_height;

    template <typename inttype, unsigned width, unsigned height>
    inline void draw_char_helper(const inttype *char_bmap, rgba_t color, unsigned x, unsigned y) {
        for (unsigned row = 0; row < height; row++) {
            inttype pixels = char_bmap[row];
            inttype column_mask = 1 << (std::min<unsigned>(width, 8*sizeof(inttype)) - 1);

            for (unsigned column = 0; column < width; column++) {
                if (pixels & column_mask)
                    draw_pixel(color, x + column, y + row);

                column_mask >>= 1;
            }
        }
    }


#ifdef HW_GRAPHICS
    char *const m_graphics_base;
    alt_u32 m_ic_id; 
    alt_u32 m_irq_id;
    volatile bool m_pending_irq;
    typedef struct {
    	unsigned X1;
    	unsigned X2;
    	unsigned Y1;
    	unsigned Y2;
    	rgba_t COLOR;
    	unsigned COMMAND;
    } CMD;
    std::queue<CMD> commands;

	static void graphics_interrupt(void *graphicsctrl);

	static constexpr unsigned DRAW_PIXEL = 1;
	static constexpr unsigned DRAW_RECT = 2;

	static constexpr unsigned X1 = 0;
	static constexpr unsigned X2 = 4;
	static constexpr unsigned Y1 = 8;
	static constexpr unsigned Y2 = 12;
	static constexpr unsigned COLOUR = 20;
	static constexpr unsigned COMMAND = 24;
	static constexpr unsigned IRQ = 28;
	static constexpr unsigned IRQ_DISABLE = 32;

    static constexpr unsigned queue_max = 64;

#endif
};

constexpr SimpleGraphics::rgba_t rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    constexpr unsigned pixel_bits = sizeof(SimpleGraphics::rgba_t) * 8;
    return ((a << 3*pixel_bits/4) 
            | (r << 2*pixel_bits/4) 
            | (g << 1*pixel_bits/4) 
            | (b << 0*pixel_bits/4));
}

constexpr SimpleGraphics::rgba_t rgb(uint8_t r, uint8_t g, uint8_t b) {
    return rgba(r, g, b, 255);
}

#endif
