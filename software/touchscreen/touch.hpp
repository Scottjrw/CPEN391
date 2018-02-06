#ifndef TOUCH_HPP
#define TOUCH_HPP
#include <fstream>
#include "touch_uart.hpp"

#define TOUCH_MAX 4095

class TouchControl {
    public:

    void touch_enable();
    void touch_disable();
    void calibrate(unsigned int mode=0x0);
    TouchUart::msg_type recv();

    // Dump a message's contents completely
    static void print(TouchUart::message *msg);

    TouchControl(const char *uart_name, TouchUart::message *recv_buf):
        m_uart(uart_name, std::ios_base::in | std::ios_base::out 
                | std::ios_base::app | std::ios_base::binary),
        m_recv_buf(recv_buf)
    {}

    private:
    std::fstream m_uart;
    TouchUart::message *const m_recv_buf;

    void send(TouchUart::message &msg);

};

#endif
