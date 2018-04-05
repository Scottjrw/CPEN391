#include <iostream>
#include <cassert>
#include <algorithm>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <system_error>
#include "NIOS_Processor_Init.hpp"

static constexpr unsigned SDRAM_WIDTH = 2;

// Note that addr is a SDRAM_WIDTH addr, so +1 is actually +2 bytes
static inline void sdram_write(void *sdram_base, uint32_t addr, uint16_t data) {
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
        uintptr_t mm_reset_base_addr, unsigned mm_reset_span):
    m_datfile(sdram_datfile, std::ios_base::in | std::ios_base::ate),
    m_doneCB(nullptr), m_progressCB(nullptr), m_printCB(nullptr),
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
            if (m_printCB)
                m_printCB("Programming the NIOS Processor...");

            nios_reset(m_reset_base, true);

            usleep(25000);
            m_sdram_pos = reinterpret_cast<uint32_t *>(m_sdram_base);
            m_sdram_end = reinterpret_cast<uint32_t *>(reinterpret_cast<uintptr_t>(m_sdram_base) + m_sdram_span);
            std::cout << std::hex << "Begin addr: " << (unsigned long) m_sdram_pos << "\nEnd addr: " << m_sdram_end << std::endl << std::dec;
            m_cur_percent = 0;
            m_state = CLEAR;
            if (m_printCB)
                m_printCB("Clearing the SDRAM...");
            break;

        case CLEAR:
        {
            unsigned count = 0;
            while (count++ < (m_sdram_span / 100)) {
                if (m_sdram_pos == m_sdram_end) {
                    m_cur_percent = 0;
                    m_state = RUN;
                    if (m_printCB)
                        m_printCB("Writing Program to the NIOS...");
                    return;
                } else if ((500 * (m_sdram_pos - reinterpret_cast<uint32_t *>(m_sdram_base)) / m_sdram_span) >= m_cur_percent) {
                    if (m_progressCB)
                        m_progressCB(m_cur_percent / 2 * 5 / 3);

                    m_cur_percent += 1;
                }

                (*m_sdram_pos) = 0;
                m_sdram_pos++;
            }

            break;
        }
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
                        m_progressCB(m_cur_percent / 2 + 50);

                    m_cur_percent += 1;
                }
            } else {
                m_state = WAIT;
            }
            break;

        case WAIT:
            if (m_printCB)
                m_printCB("Release Reset on NIOS");
            usleep(25000);
            m_state = RESET2;
            break;

        case RESET2:
            usleep(25000);
            nios_reset(m_reset_base, false);

            m_state = DONE;
            break;

        case DONE:
            if (m_printCB)
                m_printCB("NIOS is Programmed");
            ev->remove(m_ref);
            if (m_doneCB)
                m_doneCB();
            break;
    }
}
