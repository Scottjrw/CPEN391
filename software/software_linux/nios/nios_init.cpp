#include <iostream>
#include <cassert>
#include <fstream>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <err.h>
#include "arm_system.h"

constexpr unsigned sdram_addr_width = 4;
constexpr unsigned sdram_width = 2;

static void sdram_write(void *sdram_base, long addr, long data) {
    uintptr_t byte_addr = reinterpret_cast<uintptr_t>(sdram_base) + addr * sdram_width;

    volatile uint8_t *pos = reinterpret_cast<uint8_t *>(byte_addr);
    volatile uint8_t *end = pos + sdram_width;
    for (; pos < end; pos++) {
        *pos = data & 0xFF;
        data >>= 8;
    }
}

static void reset_nios(void *reset_base, bool reset) {
    volatile uint32_t *addr = static_cast<uint32_t *>(reset_base) + MM_RESET_NIOS2;

    *addr = reset ? 1 : 0;
}

int main(void) {
    std::ifstream datfile("sdram.dat", std::ios_base::in | std::ios_base::ate);

    size_t datfile_size = datfile.tellg();
    datfile.seekg(0);

    int fd = open("/dev/mem", (O_RDWR|O_SYNC));
    if (fd == -1) {
        err(1, "open /dev/mem");
    }

    void *sdram_base = mmap(NULL, SDRAM_SPAN, (PROT_READ|PROT_WRITE), MAP_SHARED, fd, SDRAM_BASE);
    if (sdram_base == MAP_FAILED) {
        close(fd);
        err(1, "mmap sdram base");
    }

    void *reset_base = mmap(NULL, MM_RESET_SPAN, (PROT_READ|PROT_WRITE), MAP_SHARED, fd, MM_RESET_BASE);
    if (reset_base == MAP_FAILED) {
        munmap(sdram_base, SDRAM_SPAN);
        close(fd);
        err(1, "mmap reset base");
    }

    std::cout << "Starting..." << std::endl << std::hex;

    std::cout << "Assert Reset on NIOS..." << std::endl;
    reset_nios(reset_base, true);

    std::cout << "Writing Program to SDRAM..." << std::endl;

    unsigned cur_percent = 0;
    while (datfile && datfile.peek() != -1) {
        if (datfile.peek() != '@')
            errx(1, "@ not found, got %x, file format error\n", datfile.peek());

        datfile.get();

        std::string addr_str, data_str;
        std::getline(datfile, addr_str, ' ');
        std::getline(datfile, data_str, '\n');

        static_assert(sizeof(long) >= sdram_addr_width, "Fix this");
        long addr = std::stol(addr_str, nullptr, 16);
        if (addr < 0)
            errx(1, "Addr is negative: %ld\n", addr);

        static_assert(sizeof(long) >= sdram_width, "Fix this");
        long data = std::stol(data_str, nullptr, 16);
        if (data < 0)
            errx(1, "Data is negative: %ld\n", data);

        sdram_write(sdram_base, addr, data);

        if (100*datfile.tellg() / datfile_size >= cur_percent) {
            std::cout << std::dec << "Progress: " << cur_percent << '%' << std::hex << std::endl;
            cur_percent += 10;
        }

    }
    usleep(500000);

    std::cout << "Release Reset on NIOS..." << std::endl;
    reset_nios(reset_base, false);
    std::cout << "Done!" << std::endl;
    return 0;
}
