//
//  main.c
//  PixcelDetection
//
//  Created by Chi Liu on 2018/1/28.
//  Copyright © 2018年 Chi Liu. All rights reserved.

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include "system.h"
#include "simple_graphics.h"
#include "sys/alt_alarm.h"

#define MAX_X 320
#define MAX_Y 240
#define NUM_SLOTS 4
#define RANGE 50
#define MARKER_SIZE 5

struct rgba {
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t a;
} __attribute__((packed));

struct point {
    unsigned x;
    unsigned y;
    unsigned count;
};

void algorithm(void);

inline double absDistanceSq(double x1, double x2, double y1, double y2){
    double distance = (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2);
    return distance;
}

inline double average2Points(double p1, double p2){
    return (p1+p2)/2;
}

int main(int argc, const char * argv[]) {
    
    while(1) {
        unsigned int start = alt_nticks();
        algorithm();
        printf("Algorithm took %lu ms\n", 1000 * (alt_nticks() - start) / alt_ticks_per_second());
    }
    
    return 0;
}
struct point slots[NUM_SLOTS];
struct point prev_slots[NUM_SLOTS];

void algorithm(void) {
    volatile uint32_t *data = VIDEO_FRAME_BUFFER_BASE;
    
    memset(slots, 0, sizeof(slots));
    
    for (int y = 0; y < MAX_Y; y++) {
        for (int x = 0; x < MAX_X; x++) {
            struct rgba pixel;
            // Don't use IORD since we want the cache to work when using NIOS F
            *((uint32_t *) &pixel) = *data;
            data++;

            if (pixel.r > 100 && pixel.g < 60 && pixel.b < 60) {
                int free_slot = -1;
                bool found_slot = false;

                for (int i = 0; i < NUM_SLOTS; i++) {
                    if (slots[i].count > 0 ) {
                        double distance = absDistanceSq(slots[i].x, x, slots[i].y, y);

                        if (distance < (RANGE*RANGE)) {
                            slots[i].x = average2Points(slots[i].x, x);
                            slots[i].y = average2Points(slots[i].y, y);
                            slots[i].count++;
                            found_slot = true;
                            break;
                        }
                    } else {
                        free_slot = i;
                    }
                }

                if (!found_slot) {
                    if (free_slot == -1)
                        printf("Ran out of slots!\n");

                    slots[free_slot].x = x;
                    slots[free_slot].y = y;
                    slots[free_slot].count += 1;
                }
            }
        }
    }

    for (int i = 0; i < NUM_SLOTS; i++) {
        printf("Slot %d: x = %u, y = %u, count= %d\n", i, slots[i].x, slots[i].y, slots[i].count);

        // Undraw previous X
        if (prev_slots[i].count > 0) {
            sg_draw_x(SG_X_COORD(prev_slots[i].x, MAX_X), SG_Y_COORD(prev_slots[i].y, MAX_Y),
                    MARKER_SIZE, sg_rgba(0,0,0,0));

        }

        // Draw the new X and record the draw
        if (slots[i].count > 0) {
            sg_draw_x(SG_X_COORD(slots[i].x, MAX_X), SG_Y_COORD(slots[i].y, MAX_Y),
                    MARKER_SIZE, sg_rgba(0,255,0,255));
            prev_slots[i].x = slots[i].x;
            prev_slots[i].y = slots[i].y;
            prev_slots[i].count = slots[i].count;
        } else {
            prev_slots[i].count = 0;
        }
    }
    
}

