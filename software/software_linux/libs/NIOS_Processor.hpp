#ifndef NIOS_PROCESSOR_HPP
#define NIOS_PROCESSOR_HPP

#include <functional>
#include <type_traits>
#include "fifo_serial.hpp"
#include "nios_hps_protocol.hpp"

/*
 * NIOS Processor Communication Handler
 *
 *
 */

class NIOS_Processor {
public:
    /* 
     * Wait until the NIOS Processor is ready
     */
    void hello();

    /*
     * Start the NIOS Processor's data collection
     */
    void start();

    /*
     * Stop the NIOS Processor's data collection
     */
    void stop();

    typedef std::function<void(unsigned x, unsigned y)> DotLocationCB;
    void dot_location_cb(DotLocationCB cb) { m_dot_location_cb = cb; }

    /*
     * Non-blocking poll function
     */
    void trypoll();

    NIOS_Processor(FIFO_Serial &serial);

private:
    NIOS_HPS_Protocol::Parser m_parser;
    DotLocationCB m_dot_location_cb;

    const NIOS_HPS_Protocol::message *wait_for_msg(NIOS_HPS_Protocol::Application app,
            unsigned timeout_ms=0);

    void handle_msg(const NIOS_HPS_Protocol::message &msg);
};

#endif // NIOS_PROCESSOR_HPP
