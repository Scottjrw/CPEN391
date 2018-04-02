#include "Hard_Processor_System.hpp"
#include <sys/alt_alarm.h>

using namespace NIOS_HPS_Protocol;

Hard_Processor_System::Hard_Processor_System(FIFO_Serial &serial):
    m_parser(serial)
{}

void Hard_Processor_System::hello() {
    const message *msg;
    do {
        msg = wait_for_msg(App_NIOS_Control);
    } while (msg->body.nios_cmd.command != NIOS_Control::NIOS_Cmd_Hello);
}

void Hard_Processor_System::dot_location(unsigned avg_x, unsigned avg_y, unsigned min_x, unsigned min_y, 
                                              unsigned max_x, unsigned max_y) {
    Dot_Location::body body = {
        .avg_x = avg_x,
        .avg_y = avg_y,
        .min_x = min_x,
        .min_y = min_y,
        .max_x = max_x,
        .max_y = max_y
    };

    m_parser.send(App_Dot_Location, &body);
}

void Hard_Processor_System::dot_location(const PixelCluster::ClusterData &data) {
    Dot_Location::body body = {
        .avg_x = data.avg_x,
        .avg_y = data.avg_y,
        .min_x = data.min_x,
        .min_y = data.min_y,
        .max_x = data.max_x,
        .max_y = data.max_y
    };

    m_parser.send(App_Dot_Location, &body);
}

void Hard_Processor_System::print(const char *str, unsigned n) {
    if (n > 0) {
        using NIOS_Print_Data::max_length;

        // The string is larger than a packet
        while (n >= max_length) {
            m_parser.m_serial.write(static_cast<uint32_t>(App_NIOS_Print_Data));
            m_parser.m_serial.write(n);

            m_parser.m_serial.write(reinterpret_cast<const uint32_t *>(str), max_length / sizeof(uint32_t));
            str += max_length;
            n -= max_length;
        }
        
        // Remaining string does not fill a packet
        if (n > 0) {
            unsigned padding = max_length / sizeof(uint32_t);
            m_parser.m_serial.write(static_cast<uint32_t>(App_NIOS_Print_Data));
            m_parser.m_serial.write(n);

            m_parser.m_serial.write(reinterpret_cast<const uint32_t *>(str), n / sizeof(uint32_t));
            str += n - (n % sizeof(uint32_t));
            padding -= n / sizeof(uint32_t);

            // If the str isn't a multiple of 4 length, then send the extra chars
            uint32_t rem_chars = 0;
            switch (n % sizeof(uint32_t)) {
                case 3:
                    rem_chars |= str[2] << 16;
                case 2:
                    rem_chars |= str[1] << 8;
                case 1:
                    rem_chars |= str[0];
                    m_parser.m_serial.write(rem_chars);
                    padding--;
            }

            // Remainder of the packet will be filled with 0
            for (unsigned i = 0; i < padding; i++)
                m_parser.m_serial.write(0);
        }
    }
}

void Hard_Processor_System::print_sync() {
    NIOS_Print_Flush::body body = {};

    m_parser.send(App_NIOS_Print_Flush, &body);
}

void Hard_Processor_System::trypoll() {
    const message *msg = m_parser.recvAsync();

    if (msg)
        handle_msg(*msg);
}

const message *Hard_Processor_System::wait_for_msg(Application app, unsigned timeout_ms) {
    const message *msg = nullptr;

    while (true) {
        if (timeout_ms == 0) {
            msg = &m_parser.recv();
        } else {
            unsigned long end_time = alt_nticks() + timeout_ms * alt_ticks_per_second() / 1000;

            do {
                if (alt_nticks() > end_time)
                    return nullptr;

                msg = m_parser.recvAsync();
            } while (msg == nullptr);
        }

        if (msg->application == app) {
            return msg;
        } else {
            handle_msg(*msg);
        }
    }
}

void Hard_Processor_System::handle_msg(const message &msg) {
    switch (msg.application) {
        case App_NIOS_Control:
        {
            NIOS_Response::body body;

            switch (msg.body.nios_cmd.command) {
                case NIOS_Control::NIOS_Cmd_Hello: 
                    body.ack = NIOS_Response::NIOS_ACK_Helloback;
                    break;
                case NIOS_Control::NIOS_Cmd_Start:
                    if (m_start_cb)
                        m_start_cb(this);

                    body.ack = NIOS_Response::NIOS_ACK_Started;
                    break;
                case NIOS_Control::NIOS_Cmd_Stop:
                    if (m_stop_cb)
                        m_stop_cb(this);

                    body.ack = NIOS_Response::NIOS_ACK_Stopped;
                    break;
                default:
                    assert(0);
            }

            m_parser.send(App_NIOS_Response, &body);
            break;
        }

        default:
            return;
    }
}
