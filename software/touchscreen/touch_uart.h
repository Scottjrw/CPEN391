#ifndef TOUCH_UART_H
#define TOUCH_UART_H

#include <stdint.h>

// Synchronization byte
#define TOUCH_SYNC 0x55
#define TOUCH_PEN_UP 0x80
#define TOUCH_PEN_DOWN 0x81

#define TOUCH_CMND_TOUCH_ENABLE 0x12
#define TOUCH_CMND_TOUCH_DISABLE 0x13
#define TOUCH_CMND_CALIBRATE 0x14
#define TOUCH_CMND_REG_READ 0x20
#define TOUCH_CMND_REG_WRITE 0x21
#define TOUCH_CMND_EE_READ 0x28
#define TOUCH_CMND_EE_WRITE 0x29
#define TOUCH_CMND_EE_READ_PARAMS 0x2B
#define TOUCH_CMND_EE_WRITE_PARAMS 0x23

#define TOUCH_STATUS_OK 0x0
#define TOUCH_STATUS_UNRECOGNIZED 0x1
#define TOUCH_STATUS_TIMEOUT 0x4
#define TOUCH_STATUS_EEPARAMS_ERR 0x5
#define TOUCH_STATUS_CAL_CANCEL 0xFC

struct touch_command {
    uint8_t CMND;
    uint8_t D[253];
} __attribute__((packed));

struct touch_response {
    uint8_t STATUS;
    uint8_t CMND;
    uint8_t D[252];
} __attribute__((packed));

union touch_data {
    uint8_t RAW[254];
    struct touch_command command;
    struct touch_response response;
};

struct touch_message {
    uint8_t SYNC; // Always 0x55
    uint8_t SIZE;
    union touch_data DATA;
} __attribute__((packed));

#endif
