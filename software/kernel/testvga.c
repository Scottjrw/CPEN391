#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdint.h>
#include <errno.h>

#define WIDTH 640
#define HEIGHT 480

void set_pixel(volatile uint32_t *buf, unsigned x, unsigned y, uint32_t pixel) {
    volatile uint32_t *pix_addr = buf + y * WIDTH + x;

    *pix_addr = pixel;
}

int main(void) {
    int fd = open("/dev/cpen391_vgabuffer", (O_RDWR));
    if (fd == -1) {
        printf("Failed to open vgabuffer, errno = %d\n", errno);
        return -1;
    }

    printf("file opened\n");

	unsigned size = 640 * 480 * 4;
    void *vga_buf = mmap(NULL, size, (PROT_READ|PROT_WRITE), MAP_SHARED, fd, 0);
    if (vga_buf == MAP_FAILED) {
        printf("Failed to mmap, errno = %d\n", errno);
        close(fd);
        return -1;
    }

    printf("mmap done\n");

    for (int y = 0; y < HEIGHT; y++)
        for (int x = 0; x < WIDTH; x++)
            set_pixel(vga_buf, x, y, 0x00000000);

    for (int i = 0; i < 400; i++)
        set_pixel(vga_buf, i, i, 0xFFFFFFFF);

    for (int y = 100; y < 300; y++) {
        for (int x = 400; x < 600; x++) {
            usleep(1);
            set_pixel(vga_buf, x, y, 0xFFFF0000);
        }
    }

    while(1) usleep(1000);


    munmap(vga_buf, size);
    close(fd);

    return 0;
}
