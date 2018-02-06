#include <assert.h>
#include "touch.h"

void touch_send(FILE *uart, touch_message *msg) {
    assert(msg->SIZE >= 1);

    int status = fwrite(msg, msg->SIZE + 2, 1, uart);
    
    if (status != 1) {
        printf("Touch: failed to send message, status = %d\n", status);
    }
}


void touch_recv(FILE *uart, touch_message *msg) {
    while (1) {
        uint8_t sync = fgetc(uart);
        msg->SYNC = sync;

        switch (sync) {
            case TOUCH_SYNC:
            {
                unsigned int msg_size = fgetc(uart);

                int status = fread(&msg->DATA, msg_size, 1, uart);

                if (status == 1) {
                    msg->SIZE = msg_size;
                    touch_response(msg);
                } else {
                    printf("Touch: failed to read message, status = %d\n", status);
                }

                return;
            }
            case TOUCH_PEN_UP:
            case TOUCH_PEN_DOWN:
            {
                uint8_t data[4];
                int status = fread(data, 4, 1, uart);

                if (status == 1) {
                    unsigned int x, y;
                    x = data[0] & 0x7F;
                    x += (data[1] & 0x1F) << 7;
                    y = data[2] & 0x7F;
                    y += (data[3] & 0x1F) << 7;

                    if (sync == TOUCH_PEN_UP)
                        touch_pen_up(x, y);
                    else
                        touch_pen_down(x, y);

                } else {
                    printf("Touch: failed to read message, status = %d\n", status);
                }

                return;
            }
            default:
                printf("Touch: bad sync: %x\n", sync);
        }
    }
}

