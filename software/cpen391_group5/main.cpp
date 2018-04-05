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

static constexpr unsigned Width_Out = 640;
static constexpr std::ratio<Width_Out, PixelCluster::Video_Width> Cluster_Scale;

using NClusterData = PixelCluster::NClusterData;
using ClusterData = PixelCluster::ClusterData;

int main(int argc, const char * argv[]) {
    FIFO_Serial hps_serial(HPS_NIOS_FIFO_OUT_BASE, HPS_NIOS_FIFO_OUT_CSR_BASE,
            NIOS_HPS_FIFO_IN_BASE, NIOS_HPS_FIFO_IN_CSR_BASE);

    Hard_Processor_System hps(hps_serial);

    HPS_Print_Stream hpsout(hps);

    bool done_0;
    NClusterData data_0;
    PixelCluster pixel_cluster_0(PIXEL_CLUSTER_0_BASE,
            PIXEL_CLUSTER_0_IRQ_INTERRUPT_CONTROLLER_ID, PIXEL_CLUSTER_0_IRQ, data_0);

    pixel_cluster_0.reset();
    pixel_cluster_0.compare0_enable(true, true, true);
    pixel_cluster_0.compare0_value(120, 60, 60);
    pixel_cluster_0.compare0_less_than(false, true, true);
    pixel_cluster_0.compare1_enable(false, false, false);
    pixel_cluster_0.range(50);

    bool done_1;
    NClusterData data_1;
    PixelCluster pixel_cluster_1(PIXEL_CLUSTER_1_BASE,
            PIXEL_CLUSTER_1_IRQ_INTERRUPT_CONTROLLER_ID, PIXEL_CLUSTER_1_IRQ, data_1);

    pixel_cluster_1.reset();
    pixel_cluster_1.compare0_enable(true, true, true);
    pixel_cluster_1.compare0_value(20, 110, 100);
    pixel_cluster_1.compare0_less_than(true, true, false);
    pixel_cluster_1.compare1_enable(false, false, false);
    pixel_cluster_1.range(50);

    bool send_points = false;

    hps.start_cb([&hpsout, &send_points] (auto) {
                hpsout << "Start Sending Points" << std::endl;
                send_points = true;
            });

    hps.stop_cb([&hpsout, &send_points] (auto) {
                hpsout << "Stop Sending Points" << std::endl;
                send_points = false;
            });


    Event_Loop ev;

    ClusterData found_cluster;

    ev.add([&done_0, &done_1, &pixel_cluster_0, &pixel_cluster_1,
            &data_0, &data_1, &hpsout, &found_cluster, &hps, &send_points] (Event_Loop *ev) {
        
        if (!done_0) {
            done_0 = pixel_cluster_0.is_done();
        } else if (!done_1) {
            done_1 = pixel_cluster_1.is_done();
        } else {
            //hpsout << data_0 << data_1;
            if (send_points && Algorithms::dual_color(data_0, data_1, found_cluster)) {
                found_cluster.scale(Cluster_Scale);
                hps.dot_location(found_cluster);
            }

            pixel_cluster_0.ack();
            pixel_cluster_1.ack();
        }

        if (pixel_cluster_0.irq_overflow() || pixel_cluster_1.irq_overflow()) {
            hpsout << "Algorithm cannot keep up with Pixel Cluster" << std::endl;
        }
    });
    ev.add(&hps, &Hard_Processor_System::trypoll);

    hps_serial.clear();
    hps.hello();
    hpsout << "Hello from NIOS!" << std::endl;
    pixel_cluster_0.startIRQ();
    pixel_cluster_1.startIRQ();
    pixel_cluster_0.reset();
    pixel_cluster_1.reset();
    hpsout << "Hello2 from NIOS!" << std::endl;

    ev.run();

    return 0;
}
