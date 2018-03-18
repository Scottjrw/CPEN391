#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

int main(void) {

    int fd = open("/dev/ttyAL0", O_RDWR);

    if (fd == -1) {
        printf("Failed to open serial\n");
        return -1;
    }

    write(fd, "hello", 5);

    sleep (1);

    char buffer[8];

    for (int i = 0; i < 8; i++)
        buffer[i] = '0' + i;

    printf("Read: %d\n", read(fd, buffer, 8));

    printf("Buffer: %8s\n", buffer);

    close(fd);

    return 0;
}
