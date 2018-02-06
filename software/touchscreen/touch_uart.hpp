#ifndef TOUCH_UART_H
#define TOUCH_UART_H

#include <stdint.h>

namespace TouchUart {

// Message type, also the sync byte
enum msg_type : uint8_t {
    INVALID = 0x0,
    MESSAGE = 0x55,
    PEN_UP = 0x80,
    PEN_DOWN = 0x81,
};

// Command byte for Commands
enum cmnd : uint8_t {
    TOUCH_ENABLE = 0x12,
    TOUCH_DISABLE = 0x13,
    CALIBRATE = 0x14,
    REG_READ = 0x20,
    REG_WRITE = 0x21,
    EE_READ = 0x28,
    EE_WRITE = 0x29,
    EE_READ_PARAMS = 0x2B,
    EE_WRITE_PARAMS = 0x23
};

// Status byte for Responses
enum status : uint8_t {
    OK = 0x0,
    UNRECOGNIZED = 0x1,
    TIMEOUT = 0x4,
    EEPARAMS_ERR = 0x5,
    CAL_CANCEL = 0xFC
};

struct command {
    uint8_t SIZE;
    cmnd CMND;
    uint8_t D[253];
} __attribute__((packed));

struct response {
    uint8_t SIZE;
    status STATUS;
    cmnd CMND;
    uint8_t D[252];
} __attribute__((packed));

struct touch_updown {
    uint8_t x_7_0;
    uint8_t x_11_8;
    uint8_t y_7_0;
    uint8_t y_11_8;
} __attribute__((packed));

union message_body {
    uint8_t RAW[255];
    struct response response;
    struct command command;
    struct touch_updown touch_updown;
};

struct message {
    msg_type TYPE;
    union message_body body;
};

};
#endif
