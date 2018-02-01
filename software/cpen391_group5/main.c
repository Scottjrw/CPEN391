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

typedef enum {
    NO_MOVE,
    UP,
    DOWN,
    LEFT,
    RIGHT,
    UP_LEFT,
    UP_RIGHT,
    DOWN_LEFT,
    DOWN_RIGHT
} Direction;

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

Direction pointDirection(struct point *prev, struct point *next){
    float slope = 1.0 * (next->y - prev->y) / (next->x - prev->x);
    float abs_slope = (slope >= 0) ? slope : -slope;

    if (abs_slope < 0.5) {
        if(next->x > prev->x) 
            return RIGHT;
        else
            return LEFT;
    } else if (abs_slope > 2) {
        if (next->y > prev->y)
            return UP;
        else
            return DOWN;
    } else if (slope < 0) {
        if (next->x > prev->x)
            return DOWN_RIGHT;
        else
            return UP_LEFT; 
    } else {
        if (next->x > prev->x)
            return UP_RIGHT;
        else
            return DOWN_LEFT;
    }
}

inline unsigned absDistanceSq(unsigned x1, unsigned x2, unsigned y1, unsigned y2){
    return (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2);
}

inline unsigned average2Points(unsigned p1, unsigned p2){
    return (p1+p2) / 2;
}

void find_dots(struct point *cur) {
    static struct point slots[NUM_SLOTS];
    static struct point prev_slots[NUM_SLOTS];

    volatile uint32_t *data = VIDEO_FRAME_BUFFER_BASE;
    
    memset(slots, 0, sizeof(slots));
    
    for (int y = 0; y < MAX_Y; y++) {
        for (int x = 0; x < MAX_X; x++) {
            struct rgba pixel;
            // Don't use IORD since we want the cache to work when using NIOS F
            *((uint32_t *) &pixel) = *data;
            data++;

            if (pixel.r > 120 && pixel.g < 60 && pixel.b < 60) {
                int free_slot = -1;
                bool found_slot = false;

                for (int i = 0; i < NUM_SLOTS; i++) {
                    if (slots[i].count > 0 ) {
                        unsigned distSq = absDistanceSq(slots[i].x, x, slots[i].y, y);

                        if (distSq < (RANGE*RANGE)) {
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

    unsigned int max_count = 0;
    unsigned int max_index = -1;

    for (int i = 0; i < NUM_SLOTS; i++) {
        //printf("Slot %d: x = %u, y = %u, count= %d\n", i, slots[i].x, slots[i].y, slots[i].count);

        // Undraw previous X
        if (prev_slots[i].count > 0) {
            sg_draw_x(SG_X_COORD(prev_slots[i].x, MAX_X), SG_Y_COORD(prev_slots[i].y, MAX_Y),
                    MARKER_SIZE, sg_rgba(0,0,0,0));
        }

        // Draw the new X and record the draw
        if (slots[i].count > 0) {
            sg_draw_x(SG_X_COORD(slots[i].x, MAX_X), SG_Y_COORD(slots[i].y, MAX_Y),
                    MARKER_SIZE, sg_rgba(0,255,0,255));

            // Find max
            if (slots[i].count > max_count) {
                max_count = slots[i].count;
                max_index = i;
            }

            memcpy(&prev_slots[i], &slots[i], sizeof(struct point));
        } else {
            prev_slots[i].count = 0;
        }
    }

    if (max_index != -1) {
        memcpy(cur, &slots[max_index], sizeof(struct point));

    }
}

#define JITTER_RANGE 15
#define STOP_MS 1000
#define START_MAX 0
#define ERR_MAX 10
#define MAX_MOVE 50

void gesture_action(Direction g) {
    switch (g) {
        case NO_MOVE:
            printf("NO_MOVE\n");
            break;
        case UP:
            printf("UP\n");
            break;
        case DOWN:
            printf("DOWN\n");
            break;
        case LEFT:
            printf("LEFT\n");
            break;
        case RIGHT:
            printf("RIGHT\n");
            break;
        case UP_LEFT:
            printf("UP_LEFT\n");
            break;
        case UP_RIGHT:
            printf("UP_RIGHT\n");
            break;
        case DOWN_LEFT:
            printf("DOWN_LEFT\n");
            break;
        case DOWN_RIGHT:
            printf("DOWN_RIGH\n");
            break;
    }
}

void gesture(struct point *prev, struct point *cur) {
    static enum {
        WAIT_START,
        GESTURE,
        WAIT_STOP
    } state = WAIT_START;

    static Direction last_dir = NO_MOVE;
    static Direction cur_gesture = NO_MOVE;
    static unsigned start_count = 0;
    static unsigned err_count = 0;
    static unsigned stop_begin_ticks = 0;

    Direction dir = NO_MOVE;
    unsigned distSq = absDistanceSq(prev->x, cur->x, prev->y, cur->y);

    if (distSq > (MAX_MOVE * MAX_MOVE)) {
        printf("Point Jumped\n");
        state = WAIT_START;
    } else if (distSq > (JITTER_RANGE * JITTER_RANGE)) {
        dir = pointDirection(prev, cur);
        gesture_action(dir);
    }

    switch (state) {
        case WAIT_START:
            if (dir != NO_MOVE) {
                if (start_count++ > START_MAX) {
                    printf("Start Gesture: ");
                    state = GESTURE;
                    cur_gesture = dir;
                    gesture_action(dir);
                    start_count = 0;
                }
            }

            break;
        case WAIT_STOP:
            if (dir == NO_MOVE) {
                if (1000 * (alt_nticks() - stop_begin_ticks) / alt_ticks_per_second() < STOP_MS) {
                    printf("Got Gesture: ");
                    gesture_action(cur_gesture);
                    state = WAIT_START;
                }
                break;
            } else {
                state = GESTURE;
            }
        case GESTURE:
            if (dir == NO_MOVE) {
                printf("Waiting for stop\n");
                state = WAIT_STOP;
                stop_begin_ticks = alt_nticks();
            } else if (last_dir != dir) {
                if (err_count++ > ERR_MAX) {
                    printf("Bad Gesture!\n");
                    state = WAIT_START;
                }
            }
            break;
    }

    last_dir = dir;
}

int main(int argc, const char * argv[]) {
    struct point prev, cur;
    
    while(1) {
        unsigned start = alt_nticks();
        find_dots(&cur);
        //printf("Algorithm took %lu ms\n", 1000 * (alt_nticks() - start) / alt_ticks_per_second());
        gesture(&prev, &cur);
        memcpy(&prev, &cur, sizeof(struct point));
    }
    
    return 0;
}
