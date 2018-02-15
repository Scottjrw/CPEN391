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

	    /*for(int i=0; i<10; i++){
	    	graphics.draw_pixel(SimpleGraphics::rgba(255, 0, 0, 255), 130,50+i);
	    }

	    for(int i=0; i<10; i++){
	    	graphics.draw_pixel(SimpleGraphics::rgba(255, 0, 0, 255), 130+i,50);
	    }

	    for(int i=0; i<10; i++){
	    	graphics.draw_pixel(SimpleGraphics::rgba(255, 0, 0, 255), 140,105+i);
	    }

	    for(int i=0; i<10; i++){
	    	graphics.draw_pixel(SimpleGraphics::rgba(255, 0, 0, 255), 145+i,105);
	    }*/

		graphics.startIRQ();
		for(int i=0; i<100; i+=3){
			graphics.draw_rect(SimpleGraphics::rgba(255, 0, 0, 255), 0+i,0,1+i,100);
		}
}
