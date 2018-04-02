#include <iostream>
#include <cassert>
#include <fstream>
#include <algorithm>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <err.h>
#include "NIOS_Processor_Init.hpp"

namespace NIOS_Processor_Init {
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


int program_nios(const char *sdram_datfile, 
        uintptr_t sdram_base_addr, unsigned sdram_span,
        uintptr_t mm_reset_base_addr, unsigned mm_reset_span) {
    std::ifstream datfile(sdram_datfile, std::ios_base::in | std::ios_base::ate);

    size_t datfile_size = datfile.tellg();
    datfile.seekg(0);

    int fd = open("/dev/mem", (O_RDWR|O_SYNC));
    if (fd == -1) {
        std::cerr << "failed to open /dev/mem" << std::endl;
        return errno;
    }

    void *sdram_base = mmap(NULL, sdram_span, (PROT_READ|PROT_WRITE), MAP_SHARED, fd, sdram_base_addr);
    if (sdram_base == MAP_FAILED) {
        close(fd);
        std::cerr << "failed to mmap sdram base" << std::endl;
        return errno;
    }

    void *reset_base = mmap(NULL, mm_reset_span, (PROT_READ|PROT_WRITE), MAP_SHARED, fd, mm_reset_base_addr);
    if (reset_base == MAP_FAILED) {
        munmap(sdram_base, sdram_span);
        close(fd);
        std::cerr << "failed to mmap mm reset base" << std::endl;
        return errno;
    }

    std::clog << "Starting..." << std::endl << std::hex;

    std::clog << "Assert Reset on NIOS" << std::endl;
    nios_reset(reset_base, true);

    std::clog << "Clearing the SDRAM... ";
    sdram_clear(sdram_base, sdram_span);
    std::clog << "Done" << std::endl;

    std::clog << "Writing Program to SDRAM..." << std::endl;

    unsigned cur_percent = 0;
    while (datfile && datfile.peek() != -1) {
        if (datfile.peek() != '@') {
            std::cerr << "@ not found, got %x, file format error\n" <<  datfile.peek();
            std::clog << std::dec;
            munmap(reset_base, mm_reset_span);
            munmap(sdram_base, sdram_span);
            close(fd);
            return EINVAL;
        }

        datfile.get();

        std::string addr_str, data_str;
        std::getline(datfile, addr_str, ' ');
        std::getline(datfile, data_str, '\n');

        long addr = std::stol(addr_str, nullptr, 16);

        long data = std::stol(data_str, nullptr, 16);

        sdram_write(sdram_base, addr, data);

        if (100*datfile.tellg() / datfile_size >= cur_percent) {
            std::clog << std::dec << "Progress: " << cur_percent << '%' << std::hex << std::endl;
            cur_percent += 10;
        }

    }
    usleep(100000);

    std::clog << "Release Reset on NIOS" << std::endl;
    nios_reset(reset_base, false);

    std::clog << std::dec;
    munmap(reset_base, mm_reset_span);
    munmap(sdram_base, sdram_span);
    close(fd);
    std::clog << "Everything Done!" << std::endl;
    return 0;
}

};
