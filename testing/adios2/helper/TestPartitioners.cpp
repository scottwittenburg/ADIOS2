/*
 * Distributed under the OSI-approved Apache License, Version 2.0.  See
 * accompanying file Copyright.txt for details.
 */
#include <cmath>
#include <cstdint>
#include <cstring>

#include <iostream>
#include <limits>
#include <stdexcept>

#include <adios2.h>
#include <adios2/common/ADIOSTypes.h>
#include <adios2/helper/adiosPartitioner.h>

#include <gtest/gtest.h>

TEST(ADIOS2Partitioner, ADIOS2PartitionerGreedy)
{
    std::vector<uint64_t> dataSizes = {97, 38, 10, 98, 2, 27, 40, 0, 54, 64, 88, 2};
    uint64_t numPartitions = 4;
    adios2::helper::Partitioning result = adios2::helper::PartitionRanks(
        dataSizes, numPartitions, adios2::helper::PartitioningStrategy::GreedyNumberPartitioning);

    // expected sizes: [129, 135, 128, 128]
    ASSERT_EQ(result.m_Sizes.size(), numPartitions);
    ASSERT_EQ(result.m_Sizes[0], 129);
    ASSERT_EQ(result.m_Sizes[1], 135);
    ASSERT_EQ(result.m_Sizes[2], 128);
    ASSERT_EQ(result.m_Sizes[3], 128);

    // expected partitions: [[3, 5, 4, 11], [0, 1], [10, 6, 7], [9, 8, 2]]
    ASSERT_EQ(result.m_Partitions.size(), numPartitions);
    ASSERT_EQ(result.m_Partitions[0].size(), 4);
    ASSERT_EQ(result.m_Partitions[0][0], 3);
    ASSERT_EQ(result.m_Partitions[0][1], 5);
    ASSERT_EQ(result.m_Partitions[0][2], 4);
    ASSERT_EQ(result.m_Partitions[0][3], 11);
    ASSERT_EQ(result.m_Partitions[1].size(), 3);
    ASSERT_EQ(result.m_Partitions[1][0], 0);
    ASSERT_EQ(result.m_Partitions[1][1], 1);
    ASSERT_EQ(result.m_Partitions[1][2], 7);
    ASSERT_EQ(result.m_Partitions[2].size(), 2);
    ASSERT_EQ(result.m_Partitions[2][0], 10);
    ASSERT_EQ(result.m_Partitions[2][1], 6);
    ASSERT_EQ(result.m_Partitions[3].size(), 3);
    ASSERT_EQ(result.m_Partitions[3][0], 9);
    ASSERT_EQ(result.m_Partitions[3][1], 8);
    ASSERT_EQ(result.m_Partitions[3][2], 2);

    dataSizes = {10, 10, 10, 10, 0, 0, 0, 0, 0, 0, 0, 0};
    numPartitions = 4;
    result = adios2::helper::PartitionRanks(
        dataSizes, numPartitions, adios2::helper::PartitioningStrategy::GreedyNumberPartitioning);

    // expected partitions: [[0, 4, 8], [1, 5, 9], [2, 6, 10], [3, 7, 11]]
    ASSERT_EQ(result.m_Partitions.size(), numPartitions);
    ASSERT_EQ(result.m_Sizes.size(), numPartitions);

    for (size_t i = 0; i < result.m_Partitions.size(); ++i)
    {
        ASSERT_EQ(result.m_Partitions[i].size(), 3);
        ASSERT_EQ(result.m_Sizes[i], 10);
    }
}

TEST(ADIOS2Partitioner, ADIOS2PartitionerBasic)
{
    std::vector<uint64_t> dataSizes;
    dataSizes.resize(16);

    uint64_t numPartitions = 3;
    adios2::helper::Partitioning result = adios2::helper::PartitionRanks(
        dataSizes, numPartitions, adios2::helper::PartitioningStrategy::SizeAgnosticPartitioning);

    ASSERT_EQ(result.m_Partitions.size(), numPartitions);
    ASSERT_EQ(result.m_Partitions[0].size(), 6);
    ASSERT_EQ(result.m_Partitions[0][0], 0);
    ASSERT_EQ(result.m_Partitions[0][1], 1);
    ASSERT_EQ(result.m_Partitions[0][2], 2);
    ASSERT_EQ(result.m_Partitions[0][3], 3);
    ASSERT_EQ(result.m_Partitions[0][4], 4);
    ASSERT_EQ(result.m_Partitions[0][5], 5);
    ASSERT_EQ(result.m_Partitions[1].size(), 5);
    ASSERT_EQ(result.m_Partitions[1][0], 6);
    ASSERT_EQ(result.m_Partitions[1][1], 7);
    ASSERT_EQ(result.m_Partitions[1][2], 8);
    ASSERT_EQ(result.m_Partitions[1][3], 9);
    ASSERT_EQ(result.m_Partitions[1][4], 10);
    ASSERT_EQ(result.m_Partitions[2].size(), 5);
    ASSERT_EQ(result.m_Partitions[2][0], 11);
    ASSERT_EQ(result.m_Partitions[2][1], 12);
    ASSERT_EQ(result.m_Partitions[2][2], 13);
    ASSERT_EQ(result.m_Partitions[2][3], 14);
    ASSERT_EQ(result.m_Partitions[2][4], 15);

    numPartitions = 7;
    result = adios2::helper::PartitionRanks(
        dataSizes, numPartitions, adios2::helper::PartitioningStrategy::SizeAgnosticPartitioning);

    ASSERT_EQ(result.m_Partitions.size(), numPartitions);
    ASSERT_EQ(result.m_Partitions[0].size(), 3);
    ASSERT_EQ(result.m_Partitions[0][0], 0);
    ASSERT_EQ(result.m_Partitions[0][1], 1);
    ASSERT_EQ(result.m_Partitions[0][2], 2);
    ASSERT_EQ(result.m_Partitions[1].size(), 3);
    ASSERT_EQ(result.m_Partitions[1][0], 3);
    ASSERT_EQ(result.m_Partitions[1][1], 4);
    ASSERT_EQ(result.m_Partitions[1][2], 5);
    ASSERT_EQ(result.m_Partitions[2].size(), 2);
    ASSERT_EQ(result.m_Partitions[2][0], 6);
    ASSERT_EQ(result.m_Partitions[2][1], 7);
    ASSERT_EQ(result.m_Partitions[3].size(), 2);
    ASSERT_EQ(result.m_Partitions[3][0], 8);
    ASSERT_EQ(result.m_Partitions[3][1], 9);
    ASSERT_EQ(result.m_Partitions[4].size(), 2);
    ASSERT_EQ(result.m_Partitions[4][0], 10);
    ASSERT_EQ(result.m_Partitions[4][1], 11);
    ASSERT_EQ(result.m_Partitions[5].size(), 2);
    ASSERT_EQ(result.m_Partitions[5][0], 12);
    ASSERT_EQ(result.m_Partitions[5][1], 13);
    ASSERT_EQ(result.m_Partitions[6].size(), 2);
    ASSERT_EQ(result.m_Partitions[6][0], 14);
    ASSERT_EQ(result.m_Partitions[6][1], 15);
}

int main(int argc, char **argv)
{
    int result;
    ::testing::InitGoogleTest(&argc, argv);
    result = RUN_ALL_TESTS();

    return result;
}
