#include <stdio.h>
#include <assert.h>
#include "system.h"
#include "sys/alt_alarm.h"
#include "sys/alt_timestamp.h"
#include "touch.h"
#include "io.h"
#include "simple_graphics.h"

#define DEBOUNCE_MS 100

#define SG_MAX_WIDTH 160
#define SG_MAX_HEIGHT 120

SimpleGraphics<uint16_t, SG_MAX_WIDTH, SG_MAX_HEIGHT, TOUCH_MAX, TOUCH_MAX> graphics(reinterpret_cast<uint16_t *>(DRAW_BUFFER_BASE));

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

    graphics.fill(graphics.rgba(0, 0, 0, 0));

    // Setting switch 0 will go into calibration mode
    if (IORD_8DIRECT(SWITCH_IN_PIO_BASE, 0) & 0x1) {
        printf("Calibration Mode\n");
        printf("Touch the 4 dots in order\n");
        touch_mk_command(&send_buf, TOUCH_CMND_TOUCH_DISABLE, 1);
        touch_send(ts_uart, &send_buf);

        touch_mk_command(&send_buf, TOUCH_CMND_CALIBRATE, 2);
        send_buf.DATA.command.D[0] = 0x1; // 4 Point Calibration
        touch_send(ts_uart, &send_buf);
        touch_recv(ts_uart, &recv_buf);
        touch_print(&recv_buf);

        if (recv_buf.DATA.response.STATUS == TOUCH_STATUS_CAL_CANCEL) {
            touch_recv(ts_uart, &recv_buf);
            touch_print(&recv_buf);
        }

        for (int i = 0; i < 4; i++) {
            touch_recv(ts_uart, &recv_buf);

            if (recv_buf.DATA.response.CMND == TOUCH_CMND_CALIBRATE &&
                    recv_buf.DATA.response.STATUS == TOUCH_STATUS_OK) {

                printf("Received Point %d / 4\n", i + 1);
            } else {
                printf("Bad Response:\n");
            }
            touch_print(&recv_buf);
        }

        printf("Calibration Done\n");
    }

    touch_mk_command(&send_buf, TOUCH_CMND_TOUCH_ENABLE, 1);
    touch_send(ts_uart, &send_buf);

    while (1) {
        touch_recv(ts_uart, &recv_buf);
        if (recv_buf.SYNC == TOUCH_SYNC)
            touch_print(&recv_buf);
    }

    return 0;
}

void touch_response(touch_message *msg) {

}

uint32_t last_pen_up_time = 0;
void touch_pen_up(unsigned int x, unsigned int y) {
    static unsigned int last_x = 0, last_y = 0;

    //if (touch_debounce(&last_pen_up_time, DEBOUNCE_MS)) {
    if (touch_debounce(&last_pen_up_time, 100)) {
        unsigned int nx = TOUCH_COORD(x, SG_MAX_WIDTH), ny = TOUCH_COORD(y, SG_MAX_HEIGHT);

        printf("TOUCH UP %d %d\n", nx, ny);
    }
}
unsigned int last_x = 0, last_y = 0;

uint32_t last_pen_down_time = 0;
void touch_pen_down(unsigned int x, unsigned int y) {
    static unsigned int last_x = 0, last_y = 0;

    //if (touch_debounce(&last_pen_down_time, DEBOUNCE_MS)) {
    if (touch_debounce(&last_pen_down_time, 100)) {
        graphics.draw_x(x, y, 3, graphics.rgba(255, 0, 0, 255));
        unsigned int nx = TOUCH_COORD(x, SG_MAX_WIDTH), ny = TOUCH_COORD(y, SG_MAX_HEIGHT);

        printf("TOUCH DOWN %d %d\n", nx, ny);
    }
}

