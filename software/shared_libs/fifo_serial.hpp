#ifndef FIFO_SERIAL_HPP
#define FIFO_SERIAL_HPP

#undef __hal__ // TEMP
#ifdef __hal__
#define FIFO_NIOS
#else
#define FIFO_ARM
#endif

#include <cstdint>
#include <unistd.h>

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
 * read/write - read/write to/from a buffer, blocks until count number of words is read. Returns the number of words read or negative on error.
 * read/writeAsync - same as the blocking, but returns if operation would block. If the block happens in the middle then the number of words read is returned. On error it returns negative.
 *
 */

class FIFO_Serial {
public:
    int read(uint32_t *buf, unsigned count) {
        for (unsigned i = 0; i < count; i++) {
            while (Status_IsEmpty(read_reg(m_recv_status_base, Status_Status)));

            buf[i] = read_reg(m_recv_base, Base_Data);
        }

        return count;
    }

    int write(const uint32_t *buf, unsigned count) {
        for (unsigned i = 0; i < count; i++) {
            while (Status_IsFull(read_reg(m_send_status_base, Status_Status)));

            write_reg(m_send_base, Base_Data, buf[i]);
        }

        return count;
    }

    int readAsync(uint32_t *buf, unsigned count) {
        for (unsigned i = 0; i < count; i++) {
            if (!Status_IsEmpty(read_reg(m_recv_status_base, Status_Status))) {
                buf[i] = read_reg(m_recv_base, Base_Data);
            } else {
                return i;
            }
        }

        return count;
    }

    int writeASync(const uint32_t *buf, unsigned count) {
        for (unsigned i = 0; i < count; i++) {
            if (!Status_IsFull(read_reg(m_send_status_base, Status_Status))) {
                write_reg(m_send_base, Base_Data, buf[i]);
            } else {
                return i;
            }
        }

        return count;
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
        if (mem_fd == -1)
            throw std::system_error(std::error_code(errno, std::system_category()));

        m_recv_base = reinterpret_cast<uint32_t *>(mmap(NULL, base_size, 
                    (PROT_READ|PROT_WRITE), MAP_SHARED, mem_fd, recv_base));
        if (m_recv_base == MAP_FAILED)
            throw std::system_error(std::error_code(errno, std::system_category()));

        m_recv_status_base = reinterpret_cast<uint32_t *>(mmap(NULL, status_base_size, 
                    (PROT_READ|PROT_WRITE), MAP_SHARED, mem_fd, recv_status_base));
        if (m_recv_status_base == MAP_FAILED)
            throw std::system_error(std::error_code(errno, std::system_category()));

        m_recv_base = reinterpret_cast<uint32_t *>(mmap(NULL, base_size, 
                    (PROT_READ|PROT_WRITE), MAP_SHARED, mem_fd, send_base));
        if (m_send_base == MAP_FAILED)
            throw std::system_error(std::error_code(errno, std::system_category()));

        m_send_status_base = reinterpret_cast<uint32_t *>(mmap(NULL, status_base_size, 
                    (PROT_READ|PROT_WRITE), MAP_SHARED, mem_fd, send_status_base));
        if (m_send_status_base == MAP_FAILED)
            throw std::system_error(std::error_code(errno, std::system_category()));
    }
#endif

private:
    volatile uint32_t *m_recv_base;
    volatile uint32_t *m_recv_status_base;
    volatile uint32_t *m_send_base;
    volatile uint32_t *m_send_status_base;

#ifdef FIFO_ARM
    int mem_fd;

    // mmap sizes
    static constexpr unsigned base_size = 4;
    static constexpr unsigned status_base_size = 24;
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
