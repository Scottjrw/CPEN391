#ifndef TOUCH_HPP
#define TOUCH_HPP
#include <functional>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
// #include <sys/alt_irq.h>
// #include <sys/alt_alarm.h>
#include <chrono>
#include "touch_uart.hpp"

// University program RS232 supports interrupts, but seems really unreliable
//#define TOUCH_UP_RS232

class TouchControl {
public:
    typedef std::function<void(TouchControl *, TouchUart::message *)> MessageCB;
    typedef std::function<void(TouchControl *, unsigned int x, unsigned int y)> TouchCB;

    // Commands
    void touch_enable();
    void touch_disable();
    void calibrate(unsigned int mode=0x0);

#ifdef TOUCH_UP_RS232
    // Start interrupt based receiving, only possible with UP Uart module
    void startIRQ();
#endif

    // Wait until a message is received (or failed to receive)
    void poll();

#ifndef TOUCH_UP_RS232
    // Non-blocking receive, only possible with the unix-like interface
    void trypoll();
#endif

    // Setup callbacks
    void setMessageCB(MessageCB cb) { m_messageCB = cb; }
    void setTouchCB(TouchCB cb) { m_touchCB = cb; }

    // Dump a message's contents completely
    static void print(TouchUart::message *msg);
    // A print that can be passed as a MessageCB
    static void printCB(TouchControl *, TouchUart::message *msg) { print(msg); }

    TouchControl(const char *uart_name, uint32_t alt_irq_id=-1, uint32_t alt_ic_id=-1,
            unsigned x_max=TOUCH_MAX, unsigned y_max=TOUCH_MAX, bool debounce=true);

    int GetFd();

private:
    int m_uart; // Uart FD
    uint32_t m_irq_id;
    uint32_t m_ic_id;
    TouchUart::message m_recv_buf;
    unsigned m_recv_index;
    MessageCB m_messageCB;
    TouchCB m_touchCB;
    unsigned m_x_max, m_y_max;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_last_touch;
    bool m_debounce;

    // Number of bytes to read at once for trypoll
    static constexpr unsigned RECV_BYTES = 16;

    // Highest value returned for x/y value, min is 0
    static constexpr unsigned TOUCH_MAX = 4095;

    // Number of ms to debounce
    static constexpr unsigned TOUCH_DEBOUNCE_MS = 300;

    inline bool debounce_ok() {
        std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_from_last = (1000 * (now - m_last_touch)) ;

        if (time_from_last.count() >= TOUCH_DEBOUNCE_MS) {
            m_last_touch = now;
            return true;
        }

        return false;
    }

#ifdef ALT_ENHANCED_INTERRUPT_API_PRESENT
    static void uart_interrupt(void *touchctrl);
#else
    static void uart_interrupt(void *touchctrl, uint32_t irq_id);
#endif

    void send(const TouchUart::message &msg);
    void recv(uint8_t val);

};

#endif
