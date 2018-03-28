#include "Hard_Processor_System.hpp"
#include <sys/alt_alarm.h>

using namespace NIOS_HPS_Protocol;

Hard_Processor_System::Hard_Processor_System(FIFO_Serial &serial):
    m_parser(serial)
{}

void Hard_Processor_System::dot_location(unsigned x, unsigned y) {
    Dot_Location::body body = {
        .dot_x = x,
        .dot_y = y
    };

    m_parser.send(App_Dot_Location, &body);
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
