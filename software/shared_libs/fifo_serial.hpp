#ifndef FIFO_SERIAL_HPP
#define FIFO_SERIAL_HPP

#ifdef __hal__
#define FIFO_NIOS
#else
#define FIFO_ARM
#endif

#ifdef FIFO_DEBUG_PRINT
#include <cstdio>
#include <cstdint>
#endif
#include <unistd.h>
#include <iostream>

#ifdef FIFO_NIOS
#include "io.h"
#endif

#ifdef FIFO_ARM
#include <system_error>
#include <fcntl.h>
#include <sys/mman.h>
#endif

/* Generic class that uses two FIFOs as a message passing system.
 * It is very similar to UART only the width is 32 bits, and generally no communication error.
 *
 * The class is designed to work on both the NIOS and ARM (Linux).
 *
 * General differences:
 * NIOS - Bare metal, no VM protection, we are also provided IORD/IOWR to bypass cache
 *
 * ARM - Linux: IO is protected by VM so we need to mmap. mmap takes care of disabling the cache, just declaring as volatile is fine.
 *
 *
 * Functions:
 * read/write - read/write to/from a buffer, blocks until count number of words is read.
 * read/writeAsync - same as the blocking, but returns if operation would block. If the block happens in the middle then the number of words read is returned.
 * clear - clean out the read buffer
 *
 */

class FIFO_Serial {
public:
    inline uint32_t read() {
        while (Status_IsEmpty(read_reg(m_recv_status_base, Status_Status)));

        return read_reg(m_recv_base, Base_Data);
    }

    void read(uint32_t *buf, unsigned count) {
        for (unsigned i = 0; i < count; i++) {
            buf[i] = read();
        }
    }

    inline void write(uint32_t val) {
        while (Status_IsFull(read_reg(m_send_status_base, Status_Status)));

        write_reg(m_send_base, Base_Data, val);
    }

    void write(const uint32_t *buf, unsigned count) {
        for (unsigned i = 0; i < count; i++) {
            write(buf[i]);
        }
    }

    unsigned readAsync(uint32_t *buf, unsigned count) {
        for (unsigned i = 0; i < count; i++) {
            if (!Status_IsEmpty(read_reg(m_recv_status_base, Status_Status))) {
                buf[i] = read_reg(m_recv_base, Base_Data);
            } else {
                return i;
            }
        }

        return count;
    }

    unsigned writeASync(const uint32_t *buf, unsigned count) {
        for (unsigned i = 0; i < count; i++) {
            if (!Status_IsFull(read_reg(m_send_status_base, Status_Status))) {
                write_reg(m_send_base, Base_Data, buf[i]);
            } else {
                return i;
            }
        }

        return count;
    }

    void clear() {
        uint32_t buf[4];
        while (readAsync(buf, 4) > 0);
    }

    FIFO_Serial(uintptr_t recv_base, uintptr_t recv_status_base,
            uintptr_t send_base, uintptr_t send_status_base)
#ifdef FIFO_NIOS
        :
        m_recv_base(reinterpret_cast<uint32_t *>(recv_base)),
        m_recv_status_base(reinterpret_cast<uint32_t *>(recv_status_base)),
        m_send_base(reinterpret_cast<uint32_t *>(send_base)),
        m_send_status_base(reinterpret_cast<uint32_t *>(send_status_base))
    { }
#else 
    {
        mem_fd = open("/dev/mem", (O_RDWR | O_SYNC));
        if (mem_fd < 0)
            throw errno_error(errno, "FIFO_Serial: open /dev/mem");

        m_recv_page = mmap_regs(mem_fd, recv_base, base_size, &m_recv_base);
        if (m_recv_page == MAP_FAILED)
            throw errno_error(errno, "FIFO_Serial: mmap recv_base");

        m_recv_status_page = mmap_regs(mem_fd, recv_status_base, status_base_size, &m_recv_status_base);
        if (m_recv_status_page == MAP_FAILED)
            throw errno_error(errno, "FIFO_Serial: mmap recv_status_base");

        m_send_page = mmap_regs(mem_fd, send_base, base_size, &m_send_base);
        if (m_send_page == MAP_FAILED)
            throw errno_error(errno, "FIFO_Serial: mmap send_base");

        m_send_status_page = mmap_regs(mem_fd, send_status_base, status_base_size, &m_send_status_base);
        if (m_send_status_page == MAP_FAILED)
            throw errno_error(errno, "FIFO_Serial: mmap send_status_base");
    }

    ~FIFO_Serial() {
        munmap(m_recv_base, base_size);
        munmap(m_recv_status_base, status_base_size);
        munmap(m_send_base, base_size);
        munmap(m_send_status_base, status_base_size);
        close(mem_fd);
    }

#endif

private:
    uint32_t *m_recv_base;
    uint32_t *m_recv_status_base;
    uint32_t *m_send_base;
    uint32_t *m_send_status_base;

#ifdef FIFO_ARM
    int mem_fd;
    void *m_recv_page;
    void *m_recv_status_page;
    void *m_send_page;
    void *m_send_status_page;

    // mmap sizes
    static constexpr unsigned base_size = 4;
    static constexpr unsigned status_base_size = 24;

    static void *mmap_regs(int mem_fd, uintptr_t addr, unsigned size, uint32_t **base) {
        uintptr_t addr_aligned = addr & (~(getpagesize() - 1));
        size += addr - addr_aligned;

        void *ret = mmap(NULL, size, (PROT_READ|PROT_WRITE), MAP_SHARED, 
                mem_fd, addr_aligned);

        if (ret != MAP_FAILED)
            *base = reinterpret_cast<uint32_t *>(static_cast<uint8_t *>(ret) + addr - addr_aligned);

        return ret;
    }

    static std::system_error errno_error(int errno_val, const char *context) {
        return std::system_error(errno_val, std::system_category(), context);
    }
#endif

    // Platform independent register read/write functions
    inline uint32_t read_reg(volatile uint32_t *base, unsigned reg) {
#ifdef FIFO_NIOS
        return IORD(base, reg);
#else
        return *(base + reg);
#endif
    }

    inline void write_reg(volatile uint32_t *base, unsigned reg, uint32_t val) {
#ifdef FIFO_NIOS
        IOWR(base, reg, val);
#else
        *(base + reg) = val;
#endif
    }

    // Registers for Base
    static constexpr unsigned Base_Data = 0;

    // Registers for Status
    static constexpr unsigned Status_Fill_Level = 0;
    static constexpr unsigned Status_Status = 1;
    static constexpr unsigned Status_Event_Enable = 2;
    static constexpr unsigned Status_Int_Enable = 3;
    static constexpr unsigned Status_Almost_Full = 4;
    static constexpr unsigned Status_Almost_Empty = 5;

    static constexpr bool Status_IsFull(uint32_t status_reg) {
        return status_reg & 0x1;
    }

    static constexpr bool Status_IsEmpty(uint32_t status_reg) {
        return status_reg & 0x2;
    }
};

#endif // FIFO_SERIAL_HPP
