#include <unistd.h>
#include "system.h"
#include "io.h"

int main(void) {
    IOWR_8DIRECT(PIXEL_CLUSTER_0_BASE, 0, 0x3);
    IOWR_8DIRECT(PIXEL_CLUSTER_0_BASE, 1, 0x3);
    IOWR_8DIRECT(PIXEL_CLUSTER_0_BASE, 2, 0x3);
    IOWR_8DIRECT(PIXEL_CLUSTER_0_BASE, 3, 0x3);

    return 0;
}
