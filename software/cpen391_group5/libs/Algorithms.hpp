#ifndef ALGORITHMS_HPP
#define ALGORITHMS_HPP
#include <tuple>
#include "PixelCluster.hpp"
#include <cmath>

#define ACCEPTABLE_RANGE 30
#define ACCEPTABLE_INTENSITY 50 // number of pixels 

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
    const ClusterData *max_count(const NClusterData &data, unsigned min_count=1) {
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

    /*
     *  check the size of the bounding box: not too big/small
     *  check that 2 clusters are close to each other
     *  check the count 
     * 
     *      *********************************
     *      **  ONLY CENTER POINT UPDATED  **
     *      *********************************
     */
    const ClusterData *dual_color(const NClusterData &data0, const NClusterData &data1) {

        for (unsigned d_0 = 0; d_0 < data0.size(); d_0++) {
            for (unsigned d_1 = 0; d_1 < data1.size(); d_1++) {
                
                if(data0[d_0].count < ACCEPTABLE_INTENSITY) break;

                else{
                    
                    unsigned center0_x = data0[d_0].avg_x;
                    unsigned center0_y = data0[d_0].avg_y;
                    unsigned center1_x = data0[d_1].avg_x;
                    unsigned center1_y = data0[d_1].avg_y;

                    if(abs(center0_x - center1_x) <= ACCEPTABLE_RANGE){

                        struct ClusterData out_cluster;
                        out_cluster.avg_x = (center0_x+center1_x)/2;
                        out_cluster.avg_y = (center0_y+center1_y)/2;
                        
                        return out_cluster;
                    }
                }
            }
        }
        return nullptr;
    }



};

#endif // ALGORITHMS_HPP
