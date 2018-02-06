#ifndef TOUCH_H
#define TOUCH_H

#include <stdbool.h>
#include <stdio.h>
#include "touch_uart.hpp"
#include "sys/alt_alarm.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef struct touch_message touch_message;

// Convenience function to make a command message
static inline void touch_mk_command(touch_message *msg, uint8_t cmnd, uint8_t size) {
    msg->SYNC = TOUCH_SYNC;
    msg->SIZE = size;
    msg->DATA.command.CMND = cmnd;
}




// Convert coord from the touch controller to coordinates with range [0, max]
#define TOUCH_COORD(coord, max) ((coord * max) / TOUCH_MAX)

#define TOUCH_DEBOUNCE_ERR 16
static inline bool touch_debounce_xy(unsigned int *last_x, unsigned int *last_y,
        unsigned int x, unsigned int y) {
    int dif_x = (x - *last_x);
    dif_x = (dif_x < 0) ? -dif_x : dif_x;
    int dif_y = (y - *last_y);
    dif_y = (dif_y < 0) ? -dif_y : dif_y;

    *last_x = x;
    *last_y = y;

    if (dif_x > TOUCH_DEBOUNCE_ERR || dif_y > TOUCH_DEBOUNCE_ERR) {
        return true;
    }

    return false;
}

static inline bool touch_debounce(uint32_t *timestamp, uint32_t debounce_ms) {
    uint32_t now = alt_nticks();
    uint32_t time_from_last = (1000 * (now - *timestamp)) / alt_ticks_per_second();

    if (time_from_last >= debounce_ms) {
        *timestamp = now;
        return true;
    }

    return false;
}

// Callbacks for pen_up pen_down
extern void touch_pen_up(unsigned int x, unsigned int y);
extern void touch_pen_down(unsigned int x, unsigned int y);
// Callback for a response to a command, msg is the same one passed to touch_recv
extern void touch_response(touch_message *msg);

#ifdef __cplusplus
}
#endif

#endif
