#ifndef SIMPLE_GRAPHICS_FONTS_HPP
#define SIMPLE_GRAPHICS_FONTS_HPP

namespace SimpleGraphicsFonts {
    namespace FontData {
        extern const unsigned char Font5X7[95][7];
        extern const unsigned short int Font10X14[95][14];
        extern const unsigned short Font16X27[27*95];
        extern const unsigned int Font22X40[40*95];
        extern const unsigned int Font38X59[59*95];
    };

    /* Fonts used for Simple Graphics draw char and string
     * The value of each enum is the spacing between each character
     */
    enum FontType : unsigned {
        Font5x7 = 6,
        Font10x14 = 12,
        Font16x27 = 14,
        Font22x40 = 20,
        Font38x59 = 28
    };

};

#endif // SIMPLE_GRAPHICS_FONTS_HPP
