#ifndef VISION_H
#define VISION_H

#include <stdint.h>

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

Direction pointDirection(struct point *prev, struct point *next);
void print_gesture(Direction g);

#define DOTS_MAX_X 320
#define DOTS_MAX_Y 240
#define DOTS_NUM_SLOTS 4
#define DOTS_RANGE 50
#define DOTS_MARKER_SIZE 5

void find_dots(struct point *cur);

#define GESTURE_JITTER_RANGE 15
#define GESTURE_STOP_MS 1000
#define GESTURE_START_MAX 0
#define GESTURE_ERR_MAX 10
#define GESTURE_MAX_MOVE 50

void gesture_detect(struct point *prev, struct point *cur);

#endif
