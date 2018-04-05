#include "SimpleGraphics.hpp"
#include <cassert>

#ifdef HW_GRAPHICS
    #include <sys/alt_irq.h>
#endif

using namespace SimpleGraphicsFonts;
#ifndef HW_GRAPHICS
void SimpleGraphics::draw_rect(rgba_t color, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
    for (unsigned int i = x1; i < x2; i++)
        for (unsigned int j = y1; j < y2; j++)
            draw_pixel(color, i, j);
}
#else
void SimpleGraphics::draw_rect(rgba_t color, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {

	CMD cmd = {x1, x2, y1, y2, color, DRAW_RECT};

	this->commands.push(cmd);

	if (!m_pending_irq)
		graphics_interrupt(this);

}
#endif

void SimpleGraphics::draw_x(rgba_t color, unsigned int x, unsigned int y, unsigned radius) {
    for (int i = -(int) radius; i <= (int) radius; i++) {
        for (int j = -(int) radius; j <= (int) radius; j++) {
            if (i == j || -i == j) {
                draw_pixel(color, x + i, y + j);
            }
        }
    }
}

void SimpleGraphics::draw_char(rgba_t color, unsigned x, unsigned y, char c, FontType f) {
    using namespace SimpleGraphicsFonts;
    using namespace SimpleGraphicsFonts::FontData;

    if (x >= m_width - 10 || y >= m_height - 14)
        return;

    if (c >= ' ' && c <= '~') {
        unsigned font_index = c - ' ';

        switch (f) {
            case Font5x7:
                draw_char_helper<unsigned char, 5, 7>(Font5X7[font_index], color, x, y);
                break;
            case Font10x14:
                draw_char_helper<unsigned short, 10, 14>(Font10X14[font_index], color, x, y);
                break;
            case Font16x27:
                draw_char_helper<unsigned short, 16, 27>(Font16X27 + 27*font_index, color, x, y);
                break;

            case Font22x40:
                draw_char_helper<unsigned int, 22, 40>(Font22X40 + 40*font_index, color, x, y);
                break;

            case Font38x59:
                draw_char_helper<unsigned int, 38, 59>(Font38X59 + 59*font_index, color, x, y);
                break;
        }
        
    }
}

void SimpleGraphics::draw_string(rgba_t color, unsigned x, unsigned y, std::string str, FontType f) {
    for (unsigned i = 0; i < str.length(); i++) {
        draw_char(color, x, y, str[i], f);
        x += f;
    }
}

void SimpleGraphics::draw_string_bg(rgba_t color, unsigned x, unsigned y, int str_len, FontType f) {
    unsigned start_x = x;
    unsigned end_x = x + f * str_len + 1;
    unsigned start_y = y;
    unsigned end_y;

    switch (f) {
        case Font5x7:
            end_y = y + 7;
            break;
        case Font10x14:
            end_y = y + 14;
            break;
        case Font16x27:
            end_y = y + 27;
            break;
        case Font22x40:
            end_y = y + 40;
            break;
        case Font38x59:
            end_y = y + 59;
            break;
        default:
            assert(0);

    }

    draw_rect(color, start_x, start_y, end_x, end_y);
}

void SimpleGraphics::draw_string_centered(rgba_t color, unsigned x, unsigned y, std::string str, FontType f){
    unsigned start_x = x - f * str.length()/2;
    unsigned start_y;
    switch (f) {
        case Font5x7:
            start_y = y - 4;
            break;
        case Font10x14:
            start_y = y - 7;
            break;
        case Font16x27:
            start_y = y - 13;
            break;

        case Font22x40:
            start_y = y - 20;
            break;

        case Font38x59:
            start_y = y - 29;
            break;
        default:
            assert(0);
    }

    draw_string(color, start_x, start_y, str, f);
}

void SimpleGraphics::draw_string_bg_centered(rgba_t color, unsigned x, unsigned y, int str_len, FontType f) {
    unsigned start_x = x - f * str_len/2 - 1;
    unsigned end_x = x + f * str_len/2 + 1;
    unsigned start_y;
    unsigned end_y;

    switch (f) {
        case Font5x7:
            start_y = y - 4;
            end_y = y + 4;
            break;
        case Font10x14:
            start_y = y - 7;
            end_y = y + 7;
            break;
        case Font16x27:
            start_y = y - 14;
            end_y = y + 14;
            break;
        case Font22x40:
            start_y = y - 20;
            end_y = y + 20;
            break;
        case Font38x59:
            start_y = y - 30;
            end_y = y + 30;
            break;
        default:
            assert(0);

    }

    draw_rect(color, start_x, start_y, end_x, end_y);
}

void SimpleGraphics::clear() {
    fill(rgba(0, 0, 0, 0));
}

void SimpleGraphics::fill(rgba_t color) {
    draw_rect(color, 0, 0, m_width, m_height);
}

void SimpleGraphics::draw_logo(std::string filename, unsigned x, unsigned y){
    FILE* f = fopen(filename.c_str(), "rb");
    unsigned char info[54];
    int status = fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header
    if(status < 0)
        printf("fread fail\n");

    // extract image height and width from header
    int width = (int)((info[18])|(info[19]<<8)|(info[20]<<8*2)|(info[21]<<8*3));
    int height = (int)((info[22])|(info[23]<<8)|(info[24]<<8*2)|(info[25]<<8*3));

    //handle padding
    while((width*6%4)!=0)
        width++;

    //optimize for logo
    if(width==322)
        width = 321;

    int size = 3 * width * height;
    unsigned char* data = new unsigned char[size]; // allocate 3 bytes per pixel
    status = fread(data, sizeof(unsigned char), size, f); // read the rest of the data at once
    if(status<0)
        printf("fread fail\n");
    fclose(f);

    int k = 0;// k is the index to data
    for(unsigned j=y+height; j>y; j--){
        for(unsigned i=x; i<width+x; i++){
            draw_pixel(rgba((uint8_t)data[k+2], (uint8_t)data[k+1], (uint8_t)data[k], 255), i, j);
            k += 3;
        }
    }

    //optimize for logo
    if(width==321){
        draw_rect(rgba(255,255,255,255),x,y,x+30,y+height);
        draw_rect(rgba(255,255,255,255),x+30,y,x+60,y+32);
    }

    delete [] data;
}

#ifdef HW_GRAPHICS
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
#endif

#ifndef HW_GRAPHICS
SimpleGraphics::SimpleGraphics(unsigned int width, unsigned int height):
    m_width(width),
    m_height(height)
{ 
    int fd = open("/dev/cpen391_vgabuffer", (O_RDWR|O_SYNC));
    if (fd == -1) {
        throw std::system_error(errno, std::system_category(), "fail to open vgabuffer");
    }

    unsigned size = m_width * m_height * sizeof(rgba_t);
    m_buffer_base = reinterpret_cast<SimpleGraphics::rgba_t *>(mmap(NULL, size, (PROT_READ|PROT_WRITE), MAP_SHARED, fd, 0));
    if (m_buffer_base == MAP_FAILED) {
        throw std::system_error(errno, std::system_category(), "fail to mmap SimpleGraphics");
    }

    m_max_addr = m_buffer_base + width * height;
}
#endif
