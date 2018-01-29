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

// Debounce helper, returns true 
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
