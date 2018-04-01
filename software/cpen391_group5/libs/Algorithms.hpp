#ifndef ALGORITHMS_HPP
#define ALGORITHMS_HPP
#include <tuple>
#include "PixelCluster.hpp"

namespace Algorithms {
    /*
     * Various algorithms for determining where the dot is
     *
     * returns the cluster which is the dot
     *
     * returns nullptr if the algorithm failed
     *
     */
    using NClusterData = PixelCluster::NClusterData;
    using ClusterData = PixelCluster::ClusterData;

    /* Max Count
     *
     * Finds the cluster with the largest count, greater than min_count
     *
     */
    const ClusterData *max_count(const NClusterData &data, unsigned min_count) {
        const ClusterData *max = nullptr;
        unsigned max_count = min_count - 1;

        for (const ClusterData &d: data) {
            if (d.count > max_count) {
                max = &d;
                max_count = d.count;
            }
        }

        return max;
    }

    const ClusterData *dual_color(NClusterData &data0, NClusterData &data1) {

        for (unsigned d_0 = 0; d_0 < data0.size(); d_0++) {
            for (unsigned d_1 = 0; d_1 < data1.size(); d_1++) {
                

            }
        }


        return nullptr;
    }



};

#endif // ALGORITHMS_HPP
