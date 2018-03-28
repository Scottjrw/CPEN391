#include <iostream>
#include <functional>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include "sys/alt_alarm.h"
#include "io.h"
#include "system.h"
#include "PixelCluster.hpp"
#include "fifo_serial.hpp"
#include "Hard_Processor_System.hpp"


int main(int argc, const char * argv[]) {

    FIFO_Serial hps_serial(HPS_NIOS_FIFO_OUT_BASE, HPS_NIOS_FIFO_OUT_CSR_BASE,
            NIOS_HPS_FIFO_IN_BASE, NIOS_HPS_FIFO_IN_CSR_BASE);

    hps_serial.clear();

#if 0
    std::cout << std::hex << "Read: " << hps_serial.read() << std::endl;
    hps_serial.write(0xdede);

    while(1);
#endif

    Hard_Processor_System hps(hps_serial);

    usleep(1000000);

    hps.start_cb([] (auto) {
                std::cout << "Got a start from the HPS!" << std::endl;
            });

    hps.stop_cb([] (auto) {
                std::cout << "Got a stop from the HPS!" << std::endl;
            });

    std::cout << "NIOS is ready!" << std::endl;

    while (1) hps.trypoll();

    PixelCluster pix_cluster(reinterpret_cast<uint32_t *>(PIXEL_CLUSTER_0_BASE),
            PIXEL_CLUSTER_0_IRQ_INTERRUPT_CONTROLLER_ID, PIXEL_CLUSTER_0_IRQ);

    pix_cluster.reset();
    pix_cluster.compare_enable(true, true, true);
    pix_cluster.compare_value(150, 60, 60);
    pix_cluster.compare_less_than(false, true, true);
    pix_cluster.range(50);



    return 0;
}
