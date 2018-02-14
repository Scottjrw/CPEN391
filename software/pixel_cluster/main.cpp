#include <unistd.h>
#include <stdio.h>
#include "system.h"
#include "io.h"

int main(void) {
    IOWR_16DIRECT(PIXEL_CLUSTER_0_BASE, 0, 0xff01);
    IOWR_16DIRECT(PIXEL_CLUSTER_0_BASE, 2, 0xff01);
    IOWR_16DIRECT(PIXEL_CLUSTER_0_BASE, 6, 0xff01);

    printf("%x\n", IORD_8DIRECT(PIXEL_CLUSTER_0_BASE, 7));

    return 0;
}
