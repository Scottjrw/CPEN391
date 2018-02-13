#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "system.h"
#include "sys/alt_alarm.h"
#include "vision.hpp"
#include "main.hpp"
#include "SimpleGraphics.hpp"

int main(int argc, const char * argv[]) {
    struct point prev, cur;

    while(1) {
        //unsigned start = alt_nticks();
        find_dots(&cur);
        //printf("Algorithm took %lu ms\n", 1000 * (alt_nticks() - start) / alt_ticks_per_second());
        gesture_detect(&prev, &cur);
        memcpy(&prev, &cur, sizeof(struct point));
    }
    
    return 0;
}
