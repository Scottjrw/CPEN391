#include <iostream>
#include <unistd.h>
#include "system.h"
#include <sys/alt_irq.h>
#include "io.h"

uint32_t *Control_Reg =             ((uint32_t *) (PIXEL_CLUSTER_0_BASE));
uint32_t *Compare_En_Reg =          ((uint32_t *) (PIXEL_CLUSTER_0_BASE + 4));
uint32_t *Compare_Color_Register =  ((uint32_t *) (PIXEL_CLUSTER_0_BASE + 8));
uint32_t *Range_Register =          ((uint32_t *) (PIXEL_CLUSTER_0_BASE + 12));
uint32_t *Max_X_Register =          ((uint32_t *) (PIXEL_CLUSTER_0_BASE + 16));
uint32_t *Max_Y_Register =          ((uint32_t *) (PIXEL_CLUSTER_0_BASE + 20));
uint32_t *Max_Count_Register =      ((uint32_t *) (PIXEL_CLUSTER_0_BASE + 24));
uint32_t *Debug_0_Register =        ((uint32_t *) (PIXEL_CLUSTER_0_BASE + 28));
uint32_t *Debug_1_Register =        ((uint32_t *) (PIXEL_CLUSTER_0_BASE + 32));
uint32_t *Debug_2_Register =        ((uint32_t *) (PIXEL_CLUSTER_0_BASE + 36));
uint32_t *Debug_3_Register =        ((uint32_t *) (PIXEL_CLUSTER_0_BASE + 40));

void isr(void *) {
    std::cout << "Max X = " << IORD_32DIRECT(Max_X_Register, 0) << std::endl;
    std::cout << "Max Y = " << IORD_32DIRECT(Max_Y_Register, 0) << std::endl;
    std::cout << "Max Counter = " << IORD_32DIRECT(Max_Count_Register, 0) << std::endl;

}

void printdebug() {
    std::cout << "Max Index: " << IORD_32DIRECT(Debug_0_Register, 0) << std::endl;
    std::cout << "Debug 1: " << IORD_32DIRECT(Debug_1_Register, 0) << std::endl;
    uint32_t val1_0 = IORD_32DIRECT(Debug_2_Register, 0);
    uint32_t val3_2 = IORD_32DIRECT(Debug_3_Register, 0);

    std::cout << "Val 0: " << (val1_0 & 0xFFFF) << std::endl;
    std::cout << "Val 1: " << (val1_0 >> 16) << std::endl;
    std::cout << "Val 2: " << (val3_2 & 0xFFFF) << std::endl;
    std::cout << "Val 3: " << (val3_2 >> 16) << std::endl;
}

int main(void) {
    std::cout << "Hello" << std::endl;
    //int status = alt_ic_isr_register(PIXEL_CLUSTER_0_IRQ_INTERRUPT_CONTROLLER_ID, PIXEL_CLUSTER_0_IRQ, isr, nullptr, nullptr);

    std::cout << "Start" << std::endl;
    //if (status) std::cout << "Failed to setup ISR" << std::endl;
    
    printdebug();

    uint8_t cmp_en = 0b111;
    uint8_t cmp_less_than = 0b011;
    uint32_t cmp = cmp_en | (cmp_less_than << 8);
    std::cout << "CompareEnWR: " << cmp << std::endl;
    IOWR_32DIRECT(Compare_En_Reg, 0, cmp);

    std::cout << "CompareEn: " << IORD_32DIRECT(Compare_En_Reg, 0) << std::endl;

    uint8_t b = 60;
    uint8_t g = 60;
    uint8_t r = 120;
    uint32_t color = b | (g << 8) | (r << 16);
    IOWR_32DIRECT(Compare_Color_Register, 0, color);

    std::cout << "Compare Color: " << IORD_32DIRECT(Compare_Color_Register, 0) << std::endl;

    uint32_t range = 50;
    IOWR_32DIRECT(Range_Register, 0, range);

    std::cout << "Range: " << IORD_32DIRECT(Range_Register, 0) << std::endl;

    IOWR_32DIRECT(Control_Reg, 0, 0);

    printdebug();

    while(1) {
        isr(nullptr);
        printdebug();
        IOWR_32DIRECT(Control_Reg, 0, 0);
        usleep(1000000);

    }

}
