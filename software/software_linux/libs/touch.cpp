#include <assert.h>
#include "touch.hpp"
#include "TermiosUtil.hpp"

using namespace TouchUart;

TouchControl::TouchControl(const char *uart_name, uint32_t alt_irq_id, uint32_t alt_ic_id,
        unsigned x_max, unsigned y_max, bool debounce):
    m_uart(open(uart_name, O_RDWR | O_APPEND | O_NONBLOCK)),
    m_irq_id(alt_irq_id),
    m_ic_id(alt_ic_id),
    m_recv_buf(),
    m_recv_index(0),
    m_messageCB(nullptr),
    m_touchCB(nullptr),
    m_x_max(x_max),
    m_y_max(y_max),
    m_debounce(debounce)
{
    m_recv_buf.TYPE = INVALID;
    assert(m_uart >= 0);
    TermiosUtil::SetSpeed(m_uart, B9K);

}

void TouchControl::touch_enable() {
    message msg;

    msg.TYPE = MESSAGE;
    msg.body.command.SIZE = 1;
    msg.body.command.CMND = TOUCH_ENABLE;

    send(msg);
}

void TouchControl::touch_disable() {
    message msg;

    msg.TYPE = MESSAGE;
    msg.body.command.SIZE = 1;
    msg.body.command.CMND = TOUCH_DISABLE;

    send(msg);
}

void TouchControl::calibrate(unsigned int mode) {
    message msg;

    msg.TYPE = MESSAGE;
    msg.body.command.SIZE = 2;
    msg.body.command.CMND = CALIBRATE;
    msg.body.command.D[0] = mode;

    send(msg);
}

#ifdef TOUCH_UP_RS232
void TouchControl::startIRQ() {
    // Enable Interrupt on CPU
#ifdef ALT_ENHANCED_INTERRUPT_API_PRESENT
    assert(m_ic_id != -1);
    alt_ic_isr_register(m_ic_id, m_irq_id, uart_interrupt, this, nullptr);
#else
    alt_irq_register(m_irq_id, this, uart_interrupt);
#endif

    // Enable interrupt on UART module
    alt_up_rs232_enable_read_interrupt(m_uart);
}
#endif

void TouchControl::poll() {
    do {
#ifdef TOUCH_UP_RS232
        if (alt_up_rs232_get_used_space_in_read_FIFO(m_uart) > 0) {
            uint8_t val, parity_error;

            int status = alt_up_rs232_read_data(m_uart, &val, &parity_error);
            assert(status == 0 && parity_error == 0);

            recv(val);
        }
#else
        char val;
        int num_rd = read(m_uart, &val, 1);

        if (num_rd == 1)
            recv(val);
#endif
    } while (m_recv_index != 0);
}

#ifndef TOUCH_UP_RS232
void TouchControl::trypoll() {
    int num_rd;
    char buf[RECV_BYTES];

    while ((num_rd = read(m_uart, buf, RECV_BYTES)) > 0) {
        for (int i = 0 ; i < num_rd; i++)
            recv(buf[i]);
    }
}
#endif

void TouchControl::send(const message &msg) {
    assert(msg.body.command.SIZE >= 1);

    const char *msg_arr = reinterpret_cast<const char *>(&msg);

#ifdef TOUCH_UP_RS232
    for (unsigned i = 0; i < msg.body.command.SIZE + 2; i++) {
        while (alt_up_rs232_get_available_space_in_write_FIFO(m_uart) == 0);

        int status = alt_up_rs232_write_data(m_uart, msg_arr[i]);
        assert(status == 0);
    }
#else
    int status = write(m_uart, msg_arr, msg.body.command.SIZE + 2);
    assert(status == msg.body.command.SIZE + 2);
#endif
}

void TouchControl::recv(uint8_t val) {
    msg_type sync;
    if (m_recv_index > 0) {
        sync = m_recv_buf.TYPE;
    } else {
        sync = static_cast<msg_type>(val);
        m_recv_buf.TYPE = sync;
    }

    switch (sync) {
        case MESSAGE:
            {
                response &resp = m_recv_buf.body.response;
                if (m_recv_index > 1) { 
                    m_recv_buf.body.RAW[m_recv_index - 1] = val;

                    if (m_recv_index >= resp.SIZE + 1u) {
                        // Done receiving the message
                        if (m_messageCB != nullptr) 
                            m_messageCB(this, &m_recv_buf);
                        m_recv_index = 0;
                        m_recv_buf.TYPE = INVALID;
                        return;
                    }
                } else if (m_recv_index == 1) { 
                    // Byte 2 is the size of the message body
                    resp.SIZE = val;
                }
                break;
            }
        case PEN_UP:
        case PEN_DOWN:
            {
                touch_updown &updown = m_recv_buf.body.touch_updown;
                switch (m_recv_index) {
                    case 1:
                        updown.x = val & 0x7F;
                        break;
                    case 2:
                        updown.x |= (val & 0x1F) << 7;
                        break;
                    case 3:
                        updown.y = val & 0x7F;
                        break;
                    case 4:
                        updown.y |= (val & 0x1F) << 7;

                        if (m_touchCB != nullptr && (!m_debounce || debounce_ok()))
                            m_touchCB(this,
                                    updown.x * m_x_max / TOUCH_MAX, 
                                    updown.y * m_y_max / TOUCH_MAX);

                        m_recv_buf.TYPE = INVALID;
                        m_recv_index = 0;
                        return;
                    case 0:
                        break;
                }
                break;
            }
        default:
            std::cout << "Touch: bad sync: " << sync << std::endl;
            m_recv_buf.TYPE = INVALID;
            m_recv_index = 0;
            return;
    }

    m_recv_index++;
}


#ifdef ALT_ENHANCED_INTERRUPT_API_PRESENT
void TouchControl::uart_interrupt(void *touchctrl) {
#else
void TouchControl::uart_interrupt(void *touchctrl, uint32_t irq_id) {
    (void) irq_id;
#endif
#ifdef TOUCH_UP_RS232
    TouchControl *controller = static_cast<TouchControl *>(touchctrl);

#ifdef ALT_ENHANCED_INTERRUPT_API_PRESENT
    alt_ic_irq_disable(controller->m_ic_id, controller->m_irq_id);
#else
    alt_irq_disable(controller->m_irq_id);
#endif

    while (alt_up_rs232_get_used_space_in_read_FIFO(controller->m_uart) > 0) {
        uint8_t val, parity_error;

        int status = alt_up_rs232_read_data(controller->m_uart, &val, &parity_error);
        assert(status == 0 && parity_error == 0);

        controller->recv(val);
    }

#ifdef ALT_ENHANCED_INTERRUPT_API_PRESENT
    alt_ic_irq_enable(controller->m_ic_id, controller->m_irq_id);
#else
    alt_irq_enable(controller->m_irq_id);
#endif
#endif
}


static void touch_print(response *msg);
static void touch_print(touch_updown *msg);

void TouchControl::print(message *msg) {
    std::cout << "MESSAGE\n";
    std::cout << "TYPE: ";
    switch(msg->TYPE) {
        case INVALID:
            std::cout << "INVALID\n";
            return;
        case MESSAGE:
            std::cout << "MESSAGE (RESPONSE)\n";
            ::touch_print(&msg->body.response);
            break;
        case PEN_UP:
            std::cout << "PEN UP\n";
            ::touch_print(&msg->body.touch_updown);
            break;
        case PEN_DOWN:
            std::cout << "PEN DOWN\n";
            ::touch_print(&msg->body.touch_updown);
            break;
        default:
            std::cout << "UNKNOWN: " << static_cast<int>(msg->TYPE) << '\n' << std::endl;
            return;
    }

    std::cout << "END\n" << std::endl;
}

static void touch_print(response *response) {

    std::cout << "STATUS: ";
    switch (response->STATUS) {
        case OK:
            std::cout << "OK";
            break;
        case UNRECOGNIZED:
            std::cout << "UNRECOGNIZED";
            break;
        case TIMEOUT:
            std::cout << "TIMEOUT";
            break;
        case EEPARAMS_ERR:
            std::cout << "EEPARAMS_ERR";
            break;
        case CAL_CANCEL:
            std::cout << "CAL_CANCEL";
            break;
        default:
            std::cout << "UNKNOWN: " << static_cast<int>(response->STATUS);
    };


    std::cout << "\nCMND: ";
    switch (response->CMND) {
        case TOUCH_ENABLE:
            std::cout << "TOUCH_ENABLE";
            break;
        case TOUCH_DISABLE:
            std::cout << "TOUCH_DISABLE";
            break;
        case CALIBRATE:
            std::cout << "CALIBRATE";
            break;
        case REG_READ:
            std::cout << "REG_READ";
            break;
        case REG_WRITE:
            std::cout << "REG_WRITE";
            break;
        case EE_READ:
            std::cout << "EE_READ";
            break;
        case EE_WRITE:
            std::cout << "EE_WRITE";
            break;
        case EE_READ_PARAMS:
            std::cout << "EE_READ_PARAMS";
            break;
        case EE_WRITE_PARAMS:
            std::cout << "EE_WRITE_PARAMS";
            break;
        default:
            std::cout << "UNKNOWN: " << static_cast<int>(response->CMND);
    };
    std::cout << '\n';

    if (response->SIZE > 2) {
        std::cout << "BODY:\n";

        for (int i = 0; i < response->SIZE - 2; i++) {
            std::cout << static_cast<int>(response->D[i]) << ' ';
        }
        std::cout << '\n';
    }
}

static void touch_print(touch_updown *msg) {
    std::cout << "X: " << msg->x << "\nY: " << msg->y << '\n';
}

int TouchControl::GetFd(){
    int i = m_uart;
    return i;
}
