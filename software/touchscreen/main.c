#include <stdio.h>
#include <assert.h>
#include "system.h"
#include "sys/alt_alarm.h"
#include "sys/alt_timestamp.h"
#include "touch.h"

#define DEBOUNCE_MS 100

touch_message send_buf;
touch_message recv_buf;

FILE *ts_uart = NULL;

void print_response(touch_message *response);

int main(void) {
    printf("Starting\n");

    ts_uart = fopen(TOUCHSCREEN_UART_NAME, "r+");

    if (ts_uart == NULL) {
        printf("Failed to open UART device\n");
        while (1);
    }

    touch_mk_command(&send_buf, TOUCH_CMND_TOUCH_ENABLE, 1);
    touch_send(ts_uart, &send_buf);

    while (1) {
        touch_recv(ts_uart, &recv_buf);
    }

    return 0;
}

void touch_response(touch_message *msg) {
    touch_print(msg);
}

uint32_t last_pen_up_time = 0;
void touch_pen_up(unsigned int x, unsigned int y) {
    if (touch_debounce(&last_pen_up_time, DEBOUNCE_MS))
        printf("TOUCH UP %d %d\n", x, y);
}

uint32_t last_pen_down_time = 0;
void touch_pen_down(unsigned int x, unsigned int y) {
    if (touch_debounce(&last_pen_down_time, DEBOUNCE_MS))
        printf("TOUCH DOWN %d %d\n", x, y);
}


