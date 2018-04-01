#include "NIOS_Processor.hpp"
#include <cassert>
#include <chrono>

using namespace NIOS_HPS_Protocol;


NIOS_Processor::NIOS_Processor(FIFO_Serial &serial, std::ostream &print_stream):
    m_parser(serial),
    m_dot_location_cb(nullptr),
    m_print_enabled(true),
    m_print_buf(),
    m_ostream(print_stream)
{
    m_print_buf.reserve(PRINT_BUF_MAX);
}

int NIOS_Processor::hello(unsigned timeout_ms) {
    NIOS_Control::body body {
        .command = NIOS_Control::NIOS_Cmd_Hello,
    };

    int countdown = timeout_ms / 10;
    const message *msg;
    do {
        m_parser.send(App_NIOS_Control, &body);
        msg = wait_for_msg(App_NIOS_Response, 10);
        if (--countdown < 0)
            return -1;
    } while (msg == nullptr
            || msg->body.nios_ack.ack != NIOS_Response::NIOS_ACK_Helloback);

    return 0;
}

void NIOS_Processor::start() {
    NIOS_Control::body body {
        .command = NIOS_Control::NIOS_Cmd_Start,
    };

    m_parser.send(App_NIOS_Control, &body);

    NIOS_Response::NIOS_ACK response;
    do {
        const message *msg = wait_for_msg(App_NIOS_Response);
        response = msg->body.nios_ack.ack;
    } while (response != NIOS_Response::NIOS_ACK_Started);
}

void NIOS_Processor::stop() {
    NIOS_Control::body body {
        .command = NIOS_Control::NIOS_Cmd_Stop,
    };

    m_parser.send(App_NIOS_Control, &body);
    
    NIOS_Response::NIOS_ACK response;
    do {
        const message *msg = wait_for_msg(App_NIOS_Response);
        response = msg->body.nios_ack.ack;
    } while (response != NIOS_Response::NIOS_ACK_Stopped);
}


void NIOS_Processor::trypoll() {
    const message *msg = m_parser.recvAsync();

    if (msg)
        handle_msg(*msg);
}

const message *NIOS_Processor::wait_for_msg(Application app, unsigned timeout_ms) {
    const message *msg = nullptr;

    while (true) {
        if (timeout_ms == 0) {
            msg = &m_parser.recv();
        } else {
            auto end_time = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(timeout_ms);

            do {
                if (std::chrono::high_resolution_clock::now() > end_time)
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

void NIOS_Processor::handle_msg(const message &msg) {
    switch (msg.application) {
        case App_Dot_Location:
            if (m_dot_location_cb != nullptr)
                m_dot_location_cb(msg.body.dot_location);
            break;

        case App_NIOS_Print_Data:
            if (m_print_enabled) {
                if (m_print_buf.length() + msg.body.nios_print_data.length >= PRINT_BUF_MAX) {
                    m_ostream << NIOS_PRINT_HEADER << m_print_buf;
                    m_ostream.write(msg.body.nios_print_data.data, msg.body.nios_print_data.length);
                    m_ostream << std::endl;
                    m_print_buf.clear();
                } else {
                    m_print_buf.append(msg.body.nios_print_data.data, msg.body.nios_print_data.length);
                }
            }
            break;

        case App_NIOS_Print_Flush:
            if (m_print_enabled) {
                m_ostream << NIOS_PRINT_HEADER << m_print_buf << std::endl;
                m_print_buf.clear();
            }
            break;

        default:
            return;
    }
}
