#ifndef NIOS_HPS_PROTOCOL_HPP
#define NIOS_HPS_PROTOCOL_HPP

#include <cstdint>
#include <cassert>
#include <type_traits>
#include "fifo_serial.hpp"

/* Serial Protocol which is used between the NIOS and HPS
 *
 * The base unit of this serial is 32 bits rather than 8
 * as a result, we generally don't need to pack structs
 *
 */

namespace NIOS_HPS_Protocol {
    enum Application : uint32_t {
        App_NIOS_Control = 0xabcdef00,
        App_NIOS_Response,
        App_Dot_Location,
        App_NIOS_Print_Data,
        App_NIOS_Print_Flush,
    };

    // Control Commands from HPS to NIOS
    namespace NIOS_Control {
        enum NIOS_Command : uint32_t {
            NIOS_Cmd_Hello = 0xaa000000,
            NIOS_Cmd_Start,
            NIOS_Cmd_Stop,
        };

        struct body {
            NIOS_Command command;
        };
    };

    // Acknowledgement of Control Commands from NIOS to HPS
    namespace NIOS_Response {
        enum NIOS_ACK : uint32_t {
            NIOS_ACK_Helloback = 0xbb000000,
            NIOS_ACK_Started,
            NIOS_ACK_Stopped,
        };

        struct body {
            NIOS_ACK ack;
        };
    };

    // Dot location from NIOS to HPS
    namespace Dot_Location {
        struct body {
            uint32_t dot_x;
            uint32_t dot_y;
        };
    };

    namespace NIOS_Print_Data {
        constexpr unsigned max_length = 64;

        struct body {
            uint32_t length;
            char data[max_length];
        };
    };

    namespace NIOS_Print_Flush {
        struct body {
            uint32_t reserved;
        };
    };

    template <typename body>
    constexpr uint32_t body_length() {
        static_assert(
                std::is_same<body, NIOS_Control::body>() ||
                std::is_same<body, NIOS_Response::body>() ||
                std::is_same<body, Dot_Location::body>() ||
                std::is_same<body, NIOS_Print_Data::body>() ||
                std::is_same<body, NIOS_Print_Flush::body>()
                , "Bad body type");
        return sizeof(body) / sizeof(uint32_t);
    }

    // Should be constexpr, but the nios2-gcc seems to not support c++14 well
    inline uint32_t app_to_body_length(Application app) {
        switch (app) {
            case App_NIOS_Control:
                return sizeof(NIOS_Control::body) / sizeof(uint32_t);

            case App_NIOS_Response:
                return sizeof(NIOS_Response::body) / sizeof(uint32_t);

            case App_Dot_Location:
                return sizeof(Dot_Location::body) / sizeof(uint32_t);

            case App_NIOS_Print_Data:
                return sizeof(NIOS_Print_Data::body) / sizeof(uint32_t);

            case App_NIOS_Print_Flush:
                return sizeof(NIOS_Print_Flush::body) / sizeof(uint32_t);
        }

        return 0;
    }

    struct message {
        Application application;
        union {
            NIOS_Control::body nios_cmd;
            NIOS_Response::body nios_ack;
            Dot_Location::body dot_location;
            NIOS_Print_Data::body nios_print_data;
            NIOS_Print_Flush::body nios_print_flush;
        } body;
    };

    struct Parser {
        public:
        // Send a message
        template <typename body_type>
        void send(NIOS_HPS_Protocol::Application app, const body_type *body) {
            m_serial.write(static_cast<uint32_t>(app));
            m_serial.write(reinterpret_cast<const uint32_t *>(body),
                    NIOS_HPS_Protocol::body_length<body_type>());
        }

        // Receive a message, block until a full message is received
        const message &recv() {
            while (true) {
                while (m_remaining_words == 0) {
                    m_recv_msg.application = static_cast<Application>(m_serial.read());
                    m_remaining_words = app_to_body_length(m_recv_msg.application);
                    m_recv_msg_pos = reinterpret_cast<uint32_t *>(&m_recv_msg) + 1;
                }

                assert(m_remaining_words != 0);
                m_serial.read(m_recv_msg_pos, m_remaining_words);
                m_remaining_words = 0;

                return m_recv_msg;
            }
        }

        // Receive a message, returns if the call would block execution
        const message *recvAsync() {
            while (m_remaining_words == 0) {
                m_recv_msg.application = static_cast<Application>(m_serial.read());
                m_remaining_words = app_to_body_length(m_recv_msg.application);
                m_recv_msg_pos = reinterpret_cast<uint32_t *>(&m_recv_msg) + 1;
            }

            unsigned num_read = m_serial.readAsync(m_recv_msg_pos, m_remaining_words);
            m_remaining_words -= num_read;
            m_recv_msg_pos += num_read;

            if (m_remaining_words == 0)
                return &m_recv_msg;
            else
                return nullptr;
        }

        Parser(FIFO_Serial &serial):
            m_serial(serial),
            m_recv_msg(),
            m_recv_msg_pos(reinterpret_cast<uint32_t *>(&m_recv_msg)),
            m_remaining_words(0)
        {}

        FIFO_Serial &m_serial;

        private:
        NIOS_HPS_Protocol::message m_recv_msg;
        uint32_t *m_recv_msg_pos;
        unsigned m_remaining_words;
    };
};

#endif //NIOS_HPS_PROTOCOL_HPP
