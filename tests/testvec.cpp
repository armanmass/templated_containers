#include "gtest/gtest.h"
#include <numeric>
#include "vector.hpp"


class PopulatedVectorTest : public ::testing::Test 
{
protected:
    void SetUp() override 
    {
        vec.push_back(10);
        vec.push_back(20);
        vec.push_back(30);
        vec.push_back(40);
    }

    Vector<int> vec;
};


TEST(VectorTest, Constructors) 
{
    Vector<Vector<int>> vecs;
    vecs.push_back(Vector<int>());

    EXPECT_EQ(vecs[0].size(), 0);
    EXPECT_EQ(vecs[0].capacity(), 0);
    EXPECT_TRUE(vecs[0].empty());

    for (int i{}; i<5; ++i) vecs[0].push_back(i);

    EXPECT_EQ(vecs[0].size(), 5);
    EXPECT_EQ(vecs[0].capacity(), 8);
    EXPECT_FALSE(vecs[0].empty());

    // copy constructor test
    vecs.push_back(Vector<int>(vecs[0]));

    // copy assignment test
    vecs.push_back(Vector<int>());
    vecs[2] = vecs[1];

    for (const auto& vec : vecs)
    {
        for (int i{}; i<vec.size(); ++i)        
            EXPECT_EQ(vec[i], i);
        EXPECT_EQ(vec.size(), 5);
        EXPECT_EQ(vec.capacity(), 8);
    }

    auto vecMove{std::move(vecs[2])};
    Vector<int> vecMove2{};
    vecMove2 = std::move(vecs[1]);

    // confirm moved from (valid but unspecified state)
    for (int i{1}; i<3; ++i)
    {
        EXPECT_TRUE(vecs[i].empty());
        EXPECT_EQ(vecs[i].capacity(), 0);
        EXPECT_EQ(vecs[i].data(), nullptr);
    }

    // confirm moved to state
    for (int i{}; i<5; ++i)
    {
        EXPECT_EQ(vecMove[i], i);
        EXPECT_EQ(vecMove2[i], i);
    }

    EXPECT_EQ(vecMove.size(), 5);
    EXPECT_EQ(vecMove.capacity(), 8);
    EXPECT_EQ(vecMove2.size(), 5);
    EXPECT_EQ(vecMove2.capacity(), 8);
}


TEST_F(PopulatedVectorTest, Assign)
{
    vec.assign(2, 7);
    EXPECT_EQ(vec.size(), 2);
    // if smaller capacity should remain initial
    EXPECT_EQ(vec.capacity(), 4);
    for (int i{}; i<vec.size(); ++i)
        EXPECT_EQ(vec[i], 7);

    vec.assign(10, 7);
    EXPECT_EQ(vec.size(), 10);
    EXPECT_EQ(vec.capacity(), 10);
    for (int i{}; i<vec.size(); ++i)
        EXPECT_EQ(vec[i], 7);
}


TEST_F(PopulatedVectorTest, At) 
{
    EXPECT_THROW(vec.at(-1), std::out_of_range);
    EXPECT_THROW(vec.at(1000), std::out_of_range);
    EXPECT_EQ(vec.at(0), 10);
    EXPECT_EQ(vec.at(3), 40);

    vec.at(1) = 25;
    EXPECT_EQ(vec.at(1), 25);
}


TEST_F(PopulatedVectorTest, OperatorBracket) 
{
    EXPECT_EQ(vec[0], 10);
    EXPECT_EQ(vec[2], 30);

    vec[1] = 25;
    EXPECT_EQ(vec[1], 25);
}


TEST_F(PopulatedVectorTest, FrontAndBack) 
{
    EXPECT_EQ(vec.front(), 10);
    EXPECT_EQ(vec.back(), 40);
    
    vec.front() = 5;
    vec.back() = 45;

    EXPECT_EQ(vec.front(), 5);
    EXPECT_EQ(vec.back(), 45);
}


TEST_F(PopulatedVectorTest, IteratorBeginEnd)
{
    auto it = vec.begin();
    EXPECT_EQ(*it, 10);
    auto it_end = vec.end();
    EXPECT_NE(it, it_end);
}

TEST_F(PopulatedVectorTest, IteratorArithmetic)
{
    auto it = vec.begin();
    EXPECT_EQ(*(it + 2), 30);
    it += 3;
    EXPECT_EQ(*it, 40);
    it -= 1;
    EXPECT_EQ(*it, 30);
    EXPECT_EQ(vec.end() - vec.begin(), 4);
}

TEST_F(PopulatedVectorTest, IteratorComparison)
{
    auto it1 = vec.begin();
    auto it2 = vec.begin() + 2;
    EXPECT_TRUE(it1 < it2);
    EXPECT_TRUE(it2 > it1);
    EXPECT_TRUE(it1 != it2);
}

TEST_F(PopulatedVectorTest, IteratorModification)
{
    for (auto it = vec.begin(); it != vec.end(); ++it)
        *it += 1;

    for (int i{1}; i<5; ++i)
        EXPECT_EQ(vec[i-1], 10*i + 1);
}

TEST_F(PopulatedVectorTest, ConstIterator)
{
    const Vector<int>& const_vec = vec;
    int sum = 0;

    for (auto it = const_vec.cbegin(); it != const_vec.cend(); ++it)
        sum += *it;

    EXPECT_EQ(sum, 100);
}

TEST_F(PopulatedVectorTest, ReverseIterator)
{
    auto rit = vec.rbegin();
    EXPECT_EQ(*rit, 40);
    ++rit;
    EXPECT_EQ(*rit, 30);
    EXPECT_EQ(*(vec.rend() - 1), 10);
}

TEST_F(PopulatedVectorTest, STL_Test)
{
    long long sum = std::accumulate(vec.begin(), vec.end(), 0);
    EXPECT_EQ(sum, 100);
}


TEST(VectorTest, Data)
{
    Vector<int> v;
    ASSERT_EQ(v.data(), nullptr);

    v.push_back(100);
    v.push_back(200);

    ASSERT_NE(v.data(), nullptr);
}


TEST(VectorTest, PushBack) 
{
    Vector<std::string> v;
    EXPECT_TRUE(v.empty());
    
    std::string s1 = "hello";
    v.push_back(s1);
    EXPECT_EQ(v.size(), 1);
    EXPECT_EQ(v.back(), "hello");
    EXPECT_FALSE(v.empty());
    
    v.push_back("world");
    EXPECT_EQ(v.size(), 2);
    EXPECT_EQ(v.back(), "world");
}


TEST(VectorTest, PushBackCausesGrowth)
{
    Vector<int> v; 
    
    for (int i = 0; i < 6; ++i)
    {
        v.push_back(i);
    }
    EXPECT_EQ(v.capacity(), 8);
    EXPECT_EQ(v.size(), 6);

    
    v.push_back(6);
    v.push_back(6);
    v.push_back(6);
    EXPECT_EQ(v.size(), 9);
    EXPECT_EQ(v.capacity(), 16); 

    
    for (int i = 0; i < 7; ++i)
    {
        EXPECT_EQ(v[i], i);
    }
}

TEST(VectorTest, PopBack)
{
    Vector<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);

    EXPECT_EQ(v.size(), 3);
    EXPECT_EQ(v.back(), 3);

    v.pop_back();
    EXPECT_EQ(v.size(), 2);
    EXPECT_EQ(v.back(), 2);

    v.pop_back();
    v.pop_back();
    EXPECT_TRUE(v.empty());
}


TEST(VectorTest, Reserve)
{
    Vector<int> v;
    EXPECT_EQ(v.capacity(), 0);

    v.reserve(100);
    EXPECT_EQ(v.capacity(), 100);
    EXPECT_EQ(v.size(), 0);

    v.reserve(50);
    EXPECT_EQ(v.capacity(), 100);

    for (int i{}; i<100; ++i)
        v.push_back(i);
    EXPECT_EQ(v.capacity(), 100);
    EXPECT_EQ(v.size(), 100);
}


TEST_F(PopulatedVectorTest, ShrinkToFit)
{
    vec.reserve(100);
    EXPECT_EQ(vec.capacity(), 100);
    EXPECT_EQ(vec.size(), 4);

    vec.shrink_to_fit();
    EXPECT_EQ(vec.capacity(), 4);
    EXPECT_EQ(vec.size(), 4);
    
    for(int i{}; i<vec.size(); ++i)
        EXPECT_EQ(vec[i], 10*(i+1));
}


TEST_F(PopulatedVectorTest, Clear)
{
    EXPECT_FALSE(vec.empty());
    EXPECT_EQ(vec.size(), 4);
    EXPECT_EQ(vec.capacity(), 4);

    vec.clear();

    EXPECT_TRUE(vec.empty());
    EXPECT_EQ(vec.size(), 0);
    EXPECT_EQ(vec.capacity(), 4); 
}


