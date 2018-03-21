#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdint.h>
#include <errno.h>


int main(void) {
    const uint32_t hpsfpga_bridge = 0xC0000000;
    //const uint32_t lw_bridge = 0xFF200000;
    const uint32_t base_addr = hpsfpga_bridge + 0x300000;
    const uint32_t base_dma = hpsfpga_bridge + 0x200000;
	unsigned size = 4;
    unsigned dma_size = 4;

    int fd = open("/dev/mem", (O_RDWR));
    if (fd == -1) {
        printf("Failed to open mem, errno = %d\n", errno);
        return -1;
    }

    printf("file opened\n");

    void *base = mmap(NULL, size, (PROT_READ|PROT_WRITE), MAP_SHARED, fd, base_addr);
    if (base == MAP_FAILED) {
        printf("Failed to mmap, errno = %d\n", errno);
        return -1;
    }

    void *dma = mmap(NULL, dma_size, (PROT_READ|PROT_WRITE), MAP_SHARED, fd, base_dma);
    if (dma == MAP_FAILED) {
        printf("Failed to mmap, errno = %d\n", errno);
        return -1;
    }

    printf("mmap done\n");

    *((uint32_t *) base) = 0x0;
    printf("Reset done\n");

    while (*((volatile uint32_t *) base)) {
        printf("Still on\n");
    }

    munmap(base, size);
    close(fd);

    return 0;
}
