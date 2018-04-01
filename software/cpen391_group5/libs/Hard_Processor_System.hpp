#ifndef HARD_PROCESSOR_SYSTEM_HPP
#define HARD_PROCESSOR_SYSTEM_HPP

#include <functional>
#include <string>
#include <cstring>
#include "fifo_serial.hpp"
#include "nios_hps_protocol.hpp"

/* 
 * Hard Processor System Communication Handler
 *
 */

class Hard_Processor_System {
public:

    /*
     * Wait for a hello from the HPS
     * This needs to be called in the beginning
     */
    void hello();

    /*
     * Send a dot_location to the HPS
     */
    void dot_location(unsigned x, unsigned y, unsigned min_x, unsigned min_y, 
                                              unsigned max_x, unsigned max_y);

    /*
     * Print to the HPS's Console, these are low level functions
     * it is recommended to use HPS_Print_Stream to wrap this functionality
     *
     * print - send characters over to the HPS, once the HPS side buffer overflows it prints
     * print_sync - force the HPS to print out the characters it has now
     */
    void print(const std::string str) { print(str.c_str(), str.length()); }
    void print(const char *str, unsigned n);
    void print_sync();

    typedef std::function<void(Hard_Processor_System *)> EventCB;

    /*
     * Callback for when a start message is send
     */
    void start_cb(EventCB cb) { m_start_cb = cb; }

    /*
     * Callback for when a stop message is sent
     */
    void stop_cb(EventCB cb) { m_stop_cb = cb; }


    void trypoll();

    Hard_Processor_System(FIFO_Serial &serial);

private:
    NIOS_HPS_Protocol::Parser m_parser;
    EventCB m_start_cb;
    EventCB m_stop_cb;

    const NIOS_HPS_Protocol::message *wait_for_msg(NIOS_HPS_Protocol::Application app,
            unsigned timeout_ms=0);

    void handle_msg(const NIOS_HPS_Protocol::message &msg);
};

#endif // HARD_PROCESSOR_SYSTEM_HPP
