#include <unistd.h>
#include <stdio.h>
#include "system.h"
#include "io.h"

int main(void) {
    IOWR_16DIRECT(PIXEL_CLUSTER_0_BASE, 12, 0xccdd);

    printf("%x\n", IORD_16DIRECT(PIXEL_CLUSTER_0_BASE, 13));

    return 0;
}
