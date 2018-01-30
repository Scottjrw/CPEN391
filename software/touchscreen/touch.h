#ifndef TOUCH_H
#define TOUCH_H

#include <stdbool.h>
#include <stdio.h>
#include "touch_uart.h"
#include "sys/alt_alarm.h"

typedef struct touch_message touch_message;

// Convenience function to make a command message
static inline void touch_mk_command(touch_message *msg, uint8_t cmnd, uint8_t size) {
    msg->SYNC = TOUCH_SYNC;
    msg->SIZE = size;
    msg->DATA.command.CMND = cmnd;
}


void touch_send(FILE *uart, touch_message *msg);
void touch_recv(FILE *uart, touch_message *msg);
void touch_print(touch_message *msg);

#define TOUCH_X_MIN 0
#define TOUCH_Y_MIN 275
#define TOUCH_X_MAX 4095
#define TOUCH_Y_MAX 4095
// Convert a touch coordinate to coordinates with range [0, m_x], [0, m_y], the result is stored back into m_x, m_y
static inline void touch_coord(unsigned int x, unsigned int y, unsigned int *m_x, unsigned int *m_y) {
    *m_x = x * (*m_x) / (TOUCH_X_MAX - TOUCH_X_MIN);
    *m_y = y * (*m_y) / (TOUCH_Y_MAX - TOUCH_Y_MIN);
}

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

#endif
