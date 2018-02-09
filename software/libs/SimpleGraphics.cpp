#include "SimpleGraphics.hpp"

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

void SimpleGraphics::draw_text(unsigned x, unsigned y, rgba_t fontcolour, rgba_t backgroundcolour, int c, int Erase) {
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
