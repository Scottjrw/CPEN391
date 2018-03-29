#include <iostream>
#include <unistd.h>
#include "fifo_serial.hpp"
#include "NIOS_Processor.hpp"
#include "arm_system.h"

int main(void) {
    FIFO_Serial nios_serial(NIOS_HPS_FIFO_BASE, NIOS_HPS_FIFO_STATUS_BASE,
            HPS_NIOS_FIFO_BASE, HPS_NIOS_FIFO_STATUS_BASE);

    nios_serial.clear();

    NIOS_Processor nios(nios_serial);

    std::cout << "Wait for Hello" << std::endl;
    nios.hello();
    std::cout << "Got Hello" << std::endl;

    sleep(1);

    std::cout << "Send Start" << std::endl;
    nios.start();

    std::cout << "Send Stop" << std::endl;
    nios.stop();

    return 0;
}
