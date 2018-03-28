#include "NIOS_Processor.hpp"
#include <cassert>
#include <chrono>

using namespace NIOS_HPS_Protocol;


NIOS_Processor::NIOS_Processor(FIFO_Serial &serial):
    m_parser(serial),
    m_dot_location_cb(nullptr)
{}

void NIOS_Processor::hello() {
    NIOS_Control::body body {
        .command = NIOS_Control::NIOS_Cmd_Hello,
    };

    const message *msg;
    do {
        m_parser.send(App_NIOS_Control, &body);
        msg = wait_for_msg(App_NIOS_Response, 10);
    } while (msg == nullptr);
    assert(msg->body.nios_ack.ack == NIOS_Response::NIOS_ACK_Helloback);
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
                m_dot_location_cb(msg.body.dot_location.dot_x, msg.body.dot_location.dot_y);
            break;

        default:
            return;
    }
}
