#ifndef NIOS_INIT_HPP
#define NIOS_INIT_HPP

#include <cstdint>
#include <functional>
#include <fstream>
#include <chrono>
#include "event_loop.hpp"

class NIOS_Processor_Init {
public:

    /*
     * Program the NIOS Processor
     * sdram_datfile - name of the .dat file with the program data
     * sdram_base_addr - the base address of the sdram, get it from arm_system.h
     * sdram_span - span of the sdram, also in arm_system.h
     * mm_reset_base_addr - mm_reset base addr
     * mm_reset_span - mm_reset span
     *
     */

    typedef std::function<void(unsigned percent)> ProgressCB;
    typedef std::function<void(std::string print)> PrintCB;
    typedef std::function<void(void)> DoneCB;

    NIOS_Processor_Init(const char *sdram_datfile, 
            uintptr_t sdram_base_addr, unsigned sdram_span,
            uintptr_t mm_reset_base_addr, unsigned mm_reset_span);

    ~NIOS_Processor_Init();

    void doneCB(DoneCB doneCB) { m_doneCB = doneCB; }
    void progressCB(ProgressCB progressCB) { m_progressCB = progressCB; }
    void printCB(PrintCB printCB) { m_printCB = printCB; }

    void run(Event_Loop &ev);

    void trypoll(Event_Loop *ev);

private:
    std::ifstream m_datfile;
    DoneCB m_doneCB;
    ProgressCB m_progressCB;
    PrintCB m_printCB;
    uintptr_t m_sdram_base_addr;
    unsigned m_sdram_span;
    uintptr_t m_reset_base_addr;
    unsigned m_reset_span;
    int m_mem_fd;
    void *m_sdram_base;
    void *m_reset_base;
    size_t m_datfile_size;
    unsigned m_cur_percent;
    Event_Loop::CallableRef m_ref;
    volatile uint32_t *m_sdram_pos;
    volatile uint32_t *m_sdram_end;
    
    enum State {
        RESET1,
        CLEAR,
        RUN,
        WAIT,
        RESET2,
        DONE
    } m_state;
};


#endif // NIOS_INIT_HPP
