#include "cursor.hpp"



int main(void) {
    SimpleGraphics graphics(640, 480);

    SimpleGraphics::rgba_t bitmap[50][50] = 
#include "red_arrow_cursor.txt"
    ;

    UI::Cursor mycursor(graphics, bitmap);

    mycursor.update({100, 100});

    mycursor.draw();

    while(1);

    return 0;
}
