#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "system.h"
#include "sys/alt_alarm.h"
#include "vision.hpp"
#include "main.hpp"
#include "SimpleGraphics.hpp"
#include "touch.hpp"
#include "io.h"
#include "UI.hpp"
#include "screen.hpp"

#define DEBOUNCE_MS 100

#define SG_MAX_WIDTH 160
#define SG_MAX_HEIGHT 120

int main(int argc, const char * argv[]) {
	SimpleGraphics graphics(reinterpret_cast<SimpleGraphics::rgba_t *>(DRAW_BUFFER_BASE),
	            SG_MAX_WIDTH, SG_MAX_HEIGHT);
	    for(int i=0; i<10; i++){
	    	graphics.draw_pixel(SimpleGraphics::rgba(255, 0, 0, 255), 130,50+i);
	    }

	    graphics.draw_rect(SimpleGraphics::rgba(255, 0, 0, 255), 25,25,40,40);
	    graphics.draw_rect(SimpleGraphics::rgba(255, 0, 0, 255), 25,50,40,65);
	    /*int cur_x = IORD_32DIRECT(GRAPHICS_CONTROLLER_0_BASE, 0);
	    int cur_y = IORD_32DIRECT(GRAPHICS_CONTROLLER_0_BASE, 4);
	    int x2 = IORD_32DIRECT(GRAPHICS_CONTROLLER_0_BASE, 8);
	    int y2 = IORD_32DIRECT(GRAPHICS_CONTROLLER_0_BASE, 12);
	    int address = IORD_32DIRECT(GRAPHICS_CONTROLLER_0_BASE, 16);
	    int rec_state = IORD_32DIRECT(GRAPHICS_CONTROLLER_0_BASE, 20);
	    std::cout<<std::hex<<cur_x<<" ";
	    std::cout<<std::hex<<cur_y<<" ";
	    std::cout<<std::hex<<x2<<" ";
	    std::cout<<std::hex<<y2<<" ";
	    std::cout<<std::hex<<address<<" ";
	    std::cout<<std::hex<<rec_state<<std::endl;*/
}
