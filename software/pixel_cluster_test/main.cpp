#include <iostream>
#include <unistd.h>
#include <sys/alt_irq.h>
#include "system.h"
#include "io.h"

uint32_t &Control_Reg =             (*(uint32_t *) (PIXEL_CLUSTER_0_BASE));
uint32_t &Compare_En_Reg =          (*(uint32_t *) (PIXEL_CLUSTER_0_BASE + 4));
uint32_t &Compare_Color_Register =  (*(uint32_t *) (PIXEL_CLUSTER_0_BASE + 8));
uint32_t &Range_Register =          (*(uint32_t *) (PIXEL_CLUSTER_0_BASE + 12));
uint32_t &Max_X_Register =          (*(uint32_t *) (PIXEL_CLUSTER_0_BASE + 16));
uint32_t &Max_Y_Register =          (*(uint32_t *) (PIXEL_CLUSTER_0_BASE + 20));
uint32_t &Max_Count_Register =      (*(uint32_t *) (PIXEL_CLUSTER_0_BASE + 24));

int main(void) {

    while(1) {
        usleep(1);

    }

}
