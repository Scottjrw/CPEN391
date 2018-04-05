#ifndef ALGORITHMS_HPP
#define ALGORITHMS_HPP
#include <tuple>
#include "PixelCluster.hpp"
#include <cmath>


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

    constexpr int EXPECTED_DISTANCE = 25;
    constexpr int MAX_DISTANCE = 60;

    constexpr int EXPECTED_INTENSITY = 50;
    constexpr std::pair<int, int> INTENSITY_RANGE = {50, 200};

    constexpr int EXPECTED_WH = 50;
    constexpr std::pair<int, int> WH_RANGE = {25, 75};

    constexpr unsigned MIN_SCORE = 10;

    bool dual_color(const NClusterData &data0, const NClusterData &data1, ClusterData &found_cluster) {
        unsigned best_score = 0;

        for (const ClusterData &d_0 : data0) {
            for (const ClusterData &d_1 : data1) {
                int intensity = d_0.count + d_1.count;
                if (INTENSITY_RANGE.first <= intensity && intensity <= INTENSITY_RANGE.second) {
                    int dx = static_cast<int>(d_0.avg_x) - static_cast<int>(d_1.avg_x);
                    int dy = static_cast<int>(d_0.avg_y) - static_cast<int>(d_1.avg_y);
                    int dsq = dx * dx + dy* dy;

                    if (dsq < MAX_DISTANCE * MAX_DISTANCE) {
                        unsigned min_x = std::min(d_0.min_x, d_1.min_x);
                        unsigned min_y = std::min(d_0.min_y, d_1.min_y);
                        unsigned max_x = std::max(d_0.max_x, d_1.max_x);
                        unsigned max_y = std::max(d_0.max_y, d_1.max_y);

                        int width = max_x - min_x;
                        int height = max_y - min_y;

                        if (WH_RANGE.first <= width && width <= WH_RANGE.second &&
                                WH_RANGE.first <= height && height <= WH_RANGE.second) {

                            unsigned score =
                                  std::abs(intensity - EXPECTED_INTENSITY) * 2
                                + std::abs(dsq - EXPECTED_DISTANCE) * 4
                                + std::abs(width - EXPECTED_WH)
                                + std::abs(height - EXPECTED_WH);

                            if (score > best_score) {
                                best_score = score;
                                found_cluster.avg_x = max_x - min_x;
                                found_cluster.avg_y = max_y - min_y;
                                found_cluster.min_x = min_x;
                                found_cluster.max_x = max_x;
                                found_cluster.min_y = min_y;
                                found_cluster.max_y = max_y;
                            }
                        }
                    }
                }
            }
        }

        return best_score >= MIN_SCORE;
    }



};

#endif // ALGORITHMS_HPP
