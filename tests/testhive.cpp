#include "hive.hpp"
#include <gtest/gtest.h>
#include <vector>

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

TEST_F(HiveTest, EmplaceAndGrow)
{
    for (int i{}; i<4; ++i)
        h.emplace(i);

    EXPECT_EQ(h.size(), 4);
    EXPECT_EQ(h.capacity(), 4);

    h.emplace(4);
    EXPECT_EQ(h.size(), 5);
    EXPECT_EQ(h.capacity(), 4+8);

    auto it = h.begin();
    for (int i{}; i<5; ++i)
    {
        EXPECT_EQ(*it, i);
        ++it;
    }
    EXPECT_EQ(it, h.end());
}

TEST_F(HiveTest, Iterator)
{
    for (int i{}; i<5; ++i)
        h.emplace(i);

    int expected_val = 0;
    for (const auto& val : h)
    {
        EXPECT_EQ(val, expected_val);
        ++expected_val;
    }

    const hive<int>& const_h = h;
    EXPECT_EQ(h.begin(), const_h.begin());

    expected_val = 0;
    for (const auto& val : const_h)
    {
        EXPECT_EQ(val, expected_val);
        ++expected_val;
    }
}

TEST_F(HiveTest, FreeListTest)
{
    for (int i{}; i < 4; ++i)
        h.emplace(i);

    auto it_to_1 = ++h.begin();
    EXPECT_EQ(*it_to_1, 1);

    h.erase(it_to_1);
    EXPECT_EQ(h.size(), 3);
    EXPECT_EQ(h.capacity(), 4);

    h.emplace(99);
    EXPECT_EQ(h.size(), 4);
    EXPECT_EQ(h.capacity(), 4);

    std::vector<int> res;
    for(const auto& val : h) 
        res.push_back(val);
    
    std::vector<int> expected{0, 99, 2, 3};
    EXPECT_EQ(res, expected);
}

TEST_F(HiveTest, Clear)
{
    for (int i{}; i < 10; ++i)
        h.emplace(i);

    h.erase(h.begin());
    EXPECT_FALSE(h.is_empty());
    EXPECT_EQ(h.size(), 9);

    h.clear();
    EXPECT_TRUE(h.is_empty());
    EXPECT_EQ(h.size(), 0);
    EXPECT_EQ(h.capacity(), 0); 
    EXPECT_EQ(h.begin(), h.end());

    h.emplace(100);
    EXPECT_EQ(h.size(), 1);
    EXPECT_EQ(*h.begin(), 100);
}