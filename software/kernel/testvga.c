#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdint.h>
#include <errno.h>

#define WIDTH 160
#define HEIGHT 120

#define RXDATA_REG 0
#define TXDATA_REG 1
#define STATUS_REG 2
#define CONTROL_REG 3
#define DIVISOR_REG 4

void set_pixel(uint32_t *buf, unsigned x, unsigned y, uint32_t pixel) {
    uint32_t *pix_addr = buf + y * WIDTH + x;

    *pix_addr = pixel;
}

void write_reg(uintptr_t base, unsigned reg, uint16_t value) {
    uintptr_t reg_addr = base + 4*reg;
    printf("Writing Reg: 0x%x with value 0x%x\n" , reg_addr, value);
    *((volatile uint32_t *) reg_addr) = value;
}

uint16_t read_reg(uintptr_t base, unsigned reg) {
    uintptr_t reg_addr = base + 4*reg;
    printf("Reading Reg: 0x%x\n" , reg_addr);
    return *(volatile uint32_t *) reg_addr;
}

int main(void) {
    const uint32_t hpsfpga_bridge = 0xC0000000;
    const uint32_t lw_bridge = 0xFF200000;
    const uint32_t base_addr = lw_bridge + 0x1000;
    const uint32_t vga_size = 0x1000;

    printf("mmap 0x%x\n", base_addr);

    int mem_fd = open("/dev/mem", (O_RDWR | O_SYNC));

    if (mem_fd == -1) {
        printf("Failed to open memory, errno = %d\n", errno);
        return -1;
    }

    void *vga_buf = mmap(NULL, vga_size, (PROT_READ|PROT_WRITE), MAP_SHARED, mem_fd, base_addr);

    if (vga_buf == MAP_FAILED) {
        printf("Failed to mmap, errno = %d\n", errno);
        close(mem_fd);
        return -1;
    }

    uintptr_t vga_base_addr = (uintptr_t) vga_buf;

    //write_reg(vga_base_addr, DIVISOR_REG, 434);
    //sleep(0.25);
    //write_reg(vga_base_addr, CONTROL_REG, 1 << 9);
    //sleep(0.25);
    //write_reg(vga_base_addr, STATUS_REG, 0);
    //sleep(0.25);
    //write_reg(vga_base_addr, CONTROL_REG, 0);
    //sleep(0.25);

    //printf("STATUS REG: 0x%x\n", read_reg(vga_base_addr, STATUS_REG));
    //printf("CONTROL REG: 0x%x\n", read_reg(vga_base_addr, CONTROL_REG));
    //printf("DIVISOR REG: 0x%x\n", read_reg(vga_base_addr, DIVISOR_REG));
    //write_reg(vga_base_addr, TXDATA_REG, 0xab);
    //sleep(0.25);
    //printf("RECV %x\n", read_reg(vga_base_addr, RXDATA_REG));
    //write_reg(vga_base_addr, STATUS_REG, 0);
    //sleep(0.25);

    sleep(0.25);
    for (int i = 0; i < 8; i++)
        printf("Reg %d: 0x%x\n", i, read_reg(vga_base_addr, i));

    //sleep(0.25);
    //for (int i = 0; i < 8; i++)
    //    printf("Reg %d: 0x%x\n", i, read_reg(vga_base_addr, i));

    munmap(vga_buf, vga_size);
    close(mem_fd);

    return 0;
}
