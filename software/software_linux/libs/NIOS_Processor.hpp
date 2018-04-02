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
     * returns 0 if nios responded, not zero if it timed out
     */
    int hello(unsigned timeout_ms);

    /*
     * Start the NIOS Processor's data collection
     */
    void start();

    /*
     * Stop the NIOS Processor's data collection
     */
    void stop();

    typedef std::function<void(const NIOS_HPS_Protocol::Dot_Location::body &dot)> DotLocationCB;
    void dot_location_cb(DotLocationCB cb) { m_dot_location_cb = cb; }

    /*
     * Non-blocking poll function
     */
    void trypoll();

    /*
     * Enable/Disable printing from NIOS, enabled by default
     */
    void print(bool enable) { m_print_enabled = enable; }

    NIOS_Processor(FIFO_Serial &serial, std::ostream &print_stream = std::clog);

    ~NIOS_Processor();

private:
    NIOS_HPS_Protocol::Parser m_parser;
    DotLocationCB m_dot_location_cb;
    bool m_print_enabled;
    std::string m_print_buf;
    std::ostream &m_ostream;
    const char *m_sdram_datfile;
    void *m_reset_base;

    static constexpr unsigned PRINT_BUF_MAX = 4 * NIOS_HPS_Protocol::NIOS_Print_Data::max_length;
    static constexpr const char *NIOS_PRINT_HEADER = "[NIOS]: ";
    static constexpr unsigned SDRAM_WIDTH = 2;

    const NIOS_HPS_Protocol::message *wait_for_msg(NIOS_HPS_Protocol::Application app,
            unsigned timeout_ms=0);

    void handle_msg(const NIOS_HPS_Protocol::message &msg);
};

#endif // NIOS_PROCESSOR_HPP
