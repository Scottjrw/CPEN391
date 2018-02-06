#include <assert.h>
#include "touch.hpp"

using namespace TouchUart;

void TouchControl::touch_enable() {
    message msg;

    msg.TYPE = MESSAGE;
    msg.body.command.SIZE = 1;
    msg.body.command.CMND = TOUCH_ENABLE;

    send(msg);
}

void TouchControl::touch_disable() {
    message msg;

    msg.TYPE = MESSAGE;
    msg.body.command.SIZE = 1;
    msg.body.command.CMND = TOUCH_DISABLE;

    send(msg);
}

void TouchControl::calibrate(unsigned int mode) {
    message msg;

    msg.TYPE = MESSAGE;
    msg.body.command.SIZE = 2;
    msg.body.command.CMND = CALIBRATE;
    msg.body.command.D[0] = mode;

    send(msg);
}

void TouchControl::send(message &msg) {
    assert(msg.body.command.SIZE >= 1);

    m_uart.write(reinterpret_cast<const char *>(&msg), msg.body.command.SIZE + 2);
}

msg_type TouchControl::recv() {

}

static void touch_print(response *msg);
static void touch_print(touch_updown *msg);

void TouchControl::print(message *msg) {
    printf("MESSAGE\n");
    printf("TYPE: ");
    switch(msg->TYPE) {
        case INVALID:
            printf("INVALID\n");
            return;
        case MESSAGE:
            printf("MESSAGE\n");
            ::touch_print(&msg->body.response);
            break;
        case PEN_UP:
            printf("PEN UP\n");
            ::touch_print(&msg->body.touch_updown);
            break;
        case PEN_DOWN:
            printf("PEN DOWN\n");
            ::touch_print(&msg->body.touch_updown);
            break;
        default:
            printf("UNKNOWN: %d\n", msg->TYPE);
            return;
    }

    printf("END\n\n");
}

static void touch_print(response *response) {

    printf("STATUS: ");
    switch (response->STATUS) {
        case OK:
            printf("OK");
            break;
        case UNRECOGNIZED:
            printf("UNRECOGNIZED");
            break;
        case TIMEOUT:
            printf("TIMEOUT");
            break;
        case EEPARAMS_ERR:
            printf("EEPARAMS_ERR");
            break;
        case CAL_CANCEL:
            printf("CAL_CANCEL");
            break;
        default:
            printf("UNKNOWN: %d", response->STATUS);
    };


    printf("\nCMND: ");
    switch (response->CMND) {
        case TOUCH_ENABLE:
            printf("TOUCH_ENABLE");
            break;
        case TOUCH_DISABLE:
            printf("TOUCH_DISABLE");
            break;
        case CALIBRATE:
            printf("CALIBRATE");
            break;
        case REG_READ:
            printf("REG_READ");
            break;
        case REG_WRITE:
            printf("REG_WRITE");
            break;
        case EE_READ:
            printf("EE_READ");
            break;
        case EE_WRITE:
            printf("EE_WRITE");
            break;
        case EE_READ_PARAMS:
            printf("EE_READ_PARAMS");
            break;
        case EE_WRITE_PARAMS:
            printf("EE_WRITE_PARAMS");
            break;
        default:
            printf("UNKNOWN: %d", response->CMND);
    };
    putchar('\n');

    if (response->SIZE > 2) {
        printf("BODY:\n");

        for (int i = 0; i < response->SIZE - 2; i++) {
            printf("%x ", response->D[i]);
        }
        putchar('\n');

    }
}

static void touch_print(touch_updown *msg) {
    unsigned int x, y;
    x = msg->x_7_0 & 0x7F;
    x += (msg->x_11_8 & 0x1F) << 7;
    y = msg->y_7_0 & 0x7F;
    y += (msg->y_11_8 & 0x1F) << 7;

    printf("X: %u\nY: %u\n", x, y);
}
