#include <iostream>
#include <cassert>
#include <algorithm>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <system_error>
#include "NIOS_Processor_Init.hpp"

static constexpr unsigned SDRAM_WIDTH = 2;

static void sdram_clear(void *sdram_base, unsigned sdram_span) {
    std::uninitialized_fill_n(static_cast<uint32_t *>(sdram_base), 
            sdram_span / sizeof(uint32_t), 0);
}

static void sdram_write(void *sdram_base, uint32_t addr, uint16_t data) {
    uintptr_t byte_addr = reinterpret_cast<uintptr_t>(sdram_base) + addr * SDRAM_WIDTH;

    volatile uint8_t *pos = reinterpret_cast<uint8_t *>(byte_addr);
    volatile uint8_t *end = pos + SDRAM_WIDTH;
    for (; pos < end; pos++) {
        *pos = data & 0xFF;
        data >>= 8;
    }
}

static void nios_reset(void *reset_base, bool reset) {
    volatile uint32_t *addr = static_cast<uint32_t *>(reset_base);

    *addr = reset ? 1 : 0;
}


NIOS_Processor_Init::NIOS_Processor_Init(const char *sdram_datfile, 
        uintptr_t sdram_base_addr, unsigned sdram_span,
        uintptr_t mm_reset_base_addr, unsigned mm_reset_span,
        DoneCB doneCB, ProgressCB progressCB):
    m_datfile(sdram_datfile, std::ios_base::in | std::ios_base::ate),
    m_doneCB(doneCB), m_progressCB(progressCB),
    m_sdram_base_addr(sdram_base_addr),
    m_sdram_span(sdram_span),
    m_reset_base_addr(mm_reset_base_addr),
    m_reset_span(mm_reset_span),
    m_sdram_base(nullptr), m_reset_base(nullptr),
    m_datfile_size(m_datfile.tellg()),
    m_cur_percent(0)
{
    m_mem_fd = open("/dev/mem", (O_RDWR|O_SYNC));
    if (m_mem_fd == -1) {
        throw std::system_error(errno, std::system_category(), "failed to open /dev/mem");
    }

    m_sdram_base = mmap(NULL, sdram_span, (PROT_READ|PROT_WRITE), MAP_SHARED, m_mem_fd, sdram_base_addr);
    if (m_sdram_base == MAP_FAILED) {
        close(m_mem_fd);
        throw std::system_error(errno, std::system_category(), "failed to mmap sdram base");
    }

    m_reset_base = mmap(NULL, mm_reset_span, (PROT_READ|PROT_WRITE), MAP_SHARED, m_mem_fd, mm_reset_base_addr);
    if (m_reset_base == MAP_FAILED) {
        munmap(m_sdram_base, sdram_span);
        close(m_mem_fd);
        throw std::system_error(errno, std::system_category(), "failed to mmap mm reset base");
    }

    m_datfile.seekg(0);
}

NIOS_Processor_Init::~NIOS_Processor_Init() {
    munmap(m_reset_base, m_reset_span);
    munmap(m_sdram_base, m_sdram_span);
    close(m_mem_fd);
}

void NIOS_Processor_Init::run(Event_Loop &ev) {
    m_state = RESET1;
    m_ref = ev.add(this, &NIOS_Processor_Init::trypoll);

}

void NIOS_Processor_Init::trypoll(Event_Loop *ev) {
    switch (m_state) {
        case RESET1:
            std::clog << "Starting..." << std::endl;

            std::clog << "Assert Reset on NIOS" << std::endl;
            nios_reset(m_reset_base, true);

            std::clog << "Clearing the SDRAM... ";
            sdram_clear(m_sdram_base, m_sdram_span);
            std::clog << "Done" << std::endl;

            std::clog << "Writing Program to SDRAM..." << std::endl;
            m_state = RUN;
            break;

        case RUN:
            if (m_datfile && m_datfile.peek() != -1) {
                assert(m_datfile.peek() == '@');

                m_datfile.get();

                std::string addr_str, data_str;
                std::getline(m_datfile, addr_str, ' ');
                std::getline(m_datfile, data_str, '\n');

                long addr = std::stol(addr_str, nullptr, 16);

                long data = std::stol(data_str, nullptr, 16);

                sdram_write(m_sdram_base, addr, data);

                if (100*m_datfile.tellg() / m_datfile_size >= m_cur_percent) {
                    if (m_progressCB)
                        m_progressCB(m_cur_percent);

                    std::clog << "Progress: " << m_cur_percent << '%' << std::endl;
                    m_cur_percent += 10;
                }
            } else {
                m_state = WAIT1;
            }
            break;

        // Block less by breaking the long sleep into smaller ones
        case WAIT1:
            usleep(25000);
            m_state = WAIT2;
            break;

        case WAIT2:
            usleep(25000);
            m_state = WAIT3;
            break;

        case WAIT3:
            usleep(25000);
            m_state = WAIT4;
            break;

        case WAIT4:
            usleep(25000);
            m_state = RESET2;
            break;

        case RESET2:
            std::clog << "Release Reset on NIOS" << std::endl;
            nios_reset(m_reset_base, false);

            m_state = DONE;
            break;

        case DONE:
            ev->remove(m_ref);
            m_doneCB();
            break;
    }
}
