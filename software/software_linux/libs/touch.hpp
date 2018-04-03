#ifndef TOUCH_HPP
#define TOUCH_HPP
#include <functional>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <chrono>
#include "touch_uart.hpp"
#include "GeometricTypes.hpp"
#include "UI.hpp"

// University program RS232 supports interrupts, but seems really unreliable
//#define TOUCH_UP_RS232

class TouchControl {
public:
    typedef std::function<void(TouchControl *, TouchUart::message *)> MessageCB;
    using Touchable = UI::Touchable;
    using TouchCB = Touchable::TouchCB;

    // Commands
    void touch_enable();
    void touch_disable();
    void calibrate(unsigned int mode=0x0);

    // Wait until a message is received (or failed to receive)
    void poll();

    // Non-blocking receive, only possible with the unix-like interface
    void trypoll();

    // Setup callbacks
    void setMessageCB(MessageCB cb) { m_messageCB = cb; }

    // Set a touchable to receive the touch callbacks
    void setTouchable(Touchable &t) {
        m_touchCB = std::bind(&Touchable::touch, &t, std::placeholders::_1);
    }

    void clearTouchable() {
        m_touchCB = nullptr;
    }

    void setTouchCB(TouchCB cb) {m_touchCB = cb;}

    // Dump a message's contents completely
    static void print(TouchUart::message *msg);

    TouchControl(const char *uart_name, unsigned x_max=TOUCH_MAX, unsigned y_max=TOUCH_MAX, bool debounce=true);

private:
    int m_uart; // Uart FD
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
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_last_touch);

        if (duration.count() >= TOUCH_DEBOUNCE_MS) {
            m_last_touch = now;
            return true;
        }

        return false;
    }

    void send(const TouchUart::message &msg);
    void recv(uint8_t val);

};

#endif
