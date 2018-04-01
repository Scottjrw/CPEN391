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
#include "HPS_Print_Stream.hpp"
#include "Algorithms.hpp"
#include "event_loop.hpp"

int main(int argc, const char * argv[]) {
    FIFO_Serial hps_serial(HPS_NIOS_FIFO_OUT_BASE, HPS_NIOS_FIFO_OUT_CSR_BASE,
            NIOS_HPS_FIFO_IN_BASE, NIOS_HPS_FIFO_IN_CSR_BASE);

    Hard_Processor_System hps(hps_serial);

    HPS_Print_Stream hpsout(hps);

    PixelCluster pixel_cluster_0(PIXEL_CLUSTER_0_BASE,
            PIXEL_CLUSTER_0_IRQ_INTERRUPT_CONTROLLER_ID, PIXEL_CLUSTER_0_IRQ);

    pixel_cluster_0.reset();
    pixel_cluster_0.compare0_enable(true, true, true);
    pixel_cluster_0.compare0_value(150, 60, 60);
    pixel_cluster_0.compare0_less_than(false, true, true);
    pixel_cluster_0.compare1_enable(false, false, false);
    pixel_cluster_0.range(50);

    PixelCluster pixel_cluster_1(PIXEL_CLUSTER_1_BASE,
            PIXEL_CLUSTER_1_IRQ_INTERRUPT_CONTROLLER_ID, PIXEL_CLUSTER_1_IRQ);

    bool send_points = false;

    hps.start_cb([&hpsout, &send_points] (auto) {
                hpsout << "Start Sending Points" << std::endl;
                send_points = true;
            });

    hps.stop_cb([&hpsout, &send_points] (auto) {
                hpsout << "Stop Sending Points" << std::endl;
                send_points = false;
            });

    using NClusterData = PixelCluster::NClusterData;
    pixel_cluster_0.finish_cb([&hps, &send_points]
            (PixelCluster *, const NClusterData &data) {
                if (send_points) {
                    auto cluster = Algorithms::max_count(data, 10);

                    hps.dot_location(cluster->avg_x, cluster->avg_y,
                            cluster->min_x, cluster->min_y,
                            cluster->max_x, cluster->max_y);
                }
            });

    hps.hello();
    hpsout << "Hello from NIOS!" << std::endl;

    pixel_cluster_0.startIRQ();

    Event_Loop ev;

    ev.add(&pixel_cluster_0, &PixelCluster::trypoll);
    ev.add(&hps, &Hard_Processor_System::trypoll);
    ev.add([&pixel_cluster_0, &hpsout] (auto) {
        if (pixel_cluster_0.irq_overflow()) {
            hpsout << "Algorithm cannot keep up with Pixel Cluster" << std::endl;
        }
    });

    ev.run();

    return 0;
}
