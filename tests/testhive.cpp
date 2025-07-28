#include "hive.hpp"
#include <gtest/gtest.h>

#include <vector>
#include <numeric>
#include <algorithm>

class HiveTest : public ::testing::Test
{
protected:
    void SetUp() override
    { }

    void TearDown() override
    { }

    hive<int> h;
};

TEST_F(HiveTest, DefaultConstruct)
{
    EXPECT_TRUE(h.is_empty());
    EXPECT_EQ(h.size(), 0);
    EXPECT_EQ(h.capacity(), 0);
    EXPECT_EQ(h.begin(), h.end());
}

TEST_F(HiveTest, Insertion)
{
    for (int i{}; i < 10; ++i)
        h.emplace(5);

    EXPECT_EQ(h.size(), 10);
    EXPECT_EQ(h.capacity(), 4+8);
}

TEST_F(HiveTest, Erasure)
{
    std::vector<hive<int>::iterator> v;

    for (int i{}; i < 10; ++i)
        v.push_back(h.emplace(5));

    EXPECT_EQ(h.size(), 10);
    EXPECT_EQ(h.capacity(), 4+8);

    for (int i{}; i < 10; ++i)
    {
        h.erase(v[i]);
        EXPECT_EQ(h.size(), 10-i-1);
    }
    EXPECT_TRUE(h.is_empty());
}