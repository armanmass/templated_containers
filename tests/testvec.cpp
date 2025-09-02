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

    for (size_t i{}; i<5; ++i) vecs[0].push_back(i);

    EXPECT_EQ(vecs[0].size(), 5);
    EXPECT_EQ(vecs[0].capacity(), 8);
    EXPECT_FALSE(vecs[0].empty());

    // copy constructor test
    vecs.push_back(Vector<int>(vecs[0]));

    // copy assignment test
    vecs.push_back(Vector<int>());
    vecs[2] = vecs[1];

    for (size_t i{1}; i<vecs.size(); ++i)
    {
        for (size_t j{}; j<vecs[i].size(); ++j)        
            EXPECT_EQ(vecs[i][j], j);
        EXPECT_EQ(vecs[i].size(), 5);
        EXPECT_EQ(vecs[i].capacity(), 5);
    }

    auto vecMove{std::move(vecs[2])};
    Vector<int> vecMove2{};
    vecMove2 = std::move(vecs[1]);

    // confirm moved from (valid but unspecified state)
    for (size_t i{1}; i<3; ++i)
    {
        EXPECT_TRUE(vecs[i].empty());
        EXPECT_EQ(vecs[i].capacity(), 0);
        EXPECT_EQ(vecs[i].data(), nullptr);
    }

    // confirm moved to state
    for (size_t i{}; i<5; ++i)
    {
        EXPECT_EQ(vecMove[i], i);
        EXPECT_EQ(vecMove2[i], i);
    }

    EXPECT_EQ(vecMove.size(), 5);
    EXPECT_EQ(vecMove.capacity(), 5);
    EXPECT_EQ(vecMove2.size(), 5);
    EXPECT_EQ(vecMove2.capacity(), 5);
}


TEST_F(PopulatedVectorTest, Assign)
{
    vec.assign(2, 7);
    EXPECT_EQ(vec.size(), 2);
    // if smaller capacity should remain initial
    EXPECT_EQ(vec.capacity(), 4);
    for (size_t i{}; i<vec.size(); ++i)
        EXPECT_EQ(vec[i], 7);

    vec.assign(10, 7);
    EXPECT_EQ(vec.size(), 10);
    EXPECT_EQ(vec.capacity(), 10);
    for (size_t i{}; i<vec.size(); ++i)
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


TEST(VectorTest, Data)
{
    Vector<int> v;
    ASSERT_EQ(v.data(), nullptr);

    v.push_back(100);
    v.push_back(200);

    ASSERT_NE(v.data(), nullptr);
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

    for (size_t i{1}; i<5; ++i)
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


TEST(VectorTest, Reserve)
{
    Vector<int> v;
    EXPECT_EQ(v.capacity(), 0);

    v.reserve(100);
    EXPECT_EQ(v.capacity(), 100);
    EXPECT_EQ(v.size(), 0);

    v.reserve(50);
    EXPECT_EQ(v.capacity(), 100);

    for (size_t i{}; i<100; ++i)
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


TEST_F(PopulatedVectorTest, PushBack)
{
    // Initial state from fixture: {10, 20, 30, 40}, size 4, capacity 4
    EXPECT_EQ(vec.size(), 4);
    EXPECT_EQ(vec.capacity(), 4);

    // This push_back should trigger a reallocation
    vec.push_back(50);
    EXPECT_EQ(vec.size(), 5);
    EXPECT_EQ(vec.capacity(), 8); // 4 * 2
    EXPECT_EQ(vec.back(), 50);
    EXPECT_EQ(vec[4], 50);

    // Test pushing an lvalue
    int lvalue = 60;
    vec.push_back(lvalue);
    EXPECT_EQ(vec.size(), 6);
    EXPECT_EQ(vec.capacity(), 8);
    EXPECT_EQ(vec.back(), 60);
}

TEST_F(PopulatedVectorTest, PopBack)
{
    // Initial state: {10, 20, 30, 40}, size 4, capacity 4
    vec.pop_back();
    EXPECT_EQ(vec.size(), 3);
    EXPECT_EQ(vec.capacity(), 4); // Capacity should not change
    EXPECT_EQ(vec.back(), 30);

    vec.pop_back();
    EXPECT_EQ(vec.size(), 2);
    EXPECT_EQ(vec.back(), 20);

    vec.pop_back();
    vec.pop_back();
    EXPECT_TRUE(vec.empty());
    EXPECT_EQ(vec.size(), 0);
}

TEST_F(PopulatedVectorTest, Insert)
{
    auto it = vec.insert(vec.cbegin(), 5);
    EXPECT_EQ(*it, 5);
    EXPECT_EQ(vec.size(), 5);
    EXPECT_EQ(vec.capacity(), 8); // Should reallocate
    EXPECT_EQ(vec[0], 5);
    EXPECT_EQ(vec[1], 10);
    EXPECT_EQ(vec[2], 20);
    EXPECT_EQ(vec[3], 30);
    EXPECT_EQ(vec[4], 40);

    it = vec.insert(vec.cbegin() + 2, 15);
    EXPECT_EQ(*it, 15);
    EXPECT_EQ(vec.size(), 6);
    EXPECT_EQ(vec[0], 5);
    EXPECT_EQ(vec[1], 10);
    EXPECT_EQ(vec[2], 15);
    EXPECT_EQ(vec[3], 20);
    EXPECT_EQ(vec[4], 30);
    EXPECT_EQ(vec[5], 40);
    EXPECT_EQ(vec.back(), 40);

    it = vec.insert(vec.cend(), 50);
    EXPECT_EQ(*it, 50);
    EXPECT_EQ(vec.size(), 7);
    EXPECT_EQ(vec.back(), 50);
}

TEST(VectorTest, InsertEmpty)
{
    Vector<int> v;
    auto it = v.insert(v.cbegin(), 100);
    EXPECT_EQ(v.size(), 1);
    EXPECT_EQ(v.capacity(), 2);
    EXPECT_EQ(v.front(), 100);
    EXPECT_EQ(v.back(), 100);
    EXPECT_EQ(*it, 100);
}

TEST_F(PopulatedVectorTest, Erase)
{
    auto it = vec.erase(vec.cbegin() + 1);
    EXPECT_EQ(*it, 30);
    EXPECT_EQ(vec.size(), 3);
    EXPECT_EQ(vec[0], 10);
    EXPECT_EQ(vec[1], 30);
    EXPECT_EQ(vec[2], 40);

    it = vec.erase(vec.cbegin());
    EXPECT_EQ(*it, 30);
    EXPECT_EQ(vec.size(), 2);
    EXPECT_EQ(vec.front(), 30);

    it = vec.erase(vec.cbegin() + 1);
    EXPECT_EQ(it, vec.end());
    EXPECT_EQ(vec.size(), 1);
    EXPECT_EQ(vec.back(), 30);
}

TEST_F(PopulatedVectorTest, Resize)
{
    vec.resize(2);
    EXPECT_EQ(vec.size(), 2);
    EXPECT_EQ(vec.capacity(), 4);
    EXPECT_EQ(vec[0], 10);
    EXPECT_EQ(vec[1], 20);
    EXPECT_THROW(vec.at(2), std::out_of_range);

    vec.resize(5);
    EXPECT_EQ(vec.size(), 5);
    EXPECT_EQ(vec.capacity(), 5);
    EXPECT_EQ(vec[0], 10);
    EXPECT_EQ(vec[1], 20);
    EXPECT_EQ(vec[2], 0);
    EXPECT_EQ(vec[3], 0);
    EXPECT_EQ(vec[4], 0);

    vec.resize(0);
    EXPECT_TRUE(vec.empty());
    EXPECT_EQ(vec.capacity(), 5);
}

TEST(VectorNonMemberTest, Swap)
{
    // original
    Vector<int> vec1{10, 20, 30};
    
    // original state
    const auto* p1_before = vec1.data();
    const auto size1_before = vec1.size();
    const auto cap1_before = vec1.capacity();

    // self swap test
    swap(vec1, vec1); 

    EXPECT_EQ(vec1.data(), p1_before);
    EXPECT_EQ(vec1.size(), size1_before);
    EXPECT_EQ(vec1.capacity(), cap1_before);

    // target
    Vector<int> vec2{100, 200};

    // target state
    const auto* p2_before = vec2.data();
    const auto size2_before = vec2.size();
    const auto cap2_before = vec2.capacity();

    swap(vec1, vec2);
    
    EXPECT_EQ(vec1.data(), p2_before);
    EXPECT_EQ(vec1.size(), size2_before);
    EXPECT_EQ(vec1.capacity(), cap2_before);
    EXPECT_EQ(vec1[0], 100);
    EXPECT_EQ(vec1[1], 200);

    EXPECT_EQ(vec2.data(), p1_before);
    EXPECT_EQ(vec2.size(), size1_before);
    EXPECT_EQ(vec2.capacity(), cap1_before);
    EXPECT_EQ(vec2[0], 10);
    EXPECT_EQ(vec2[1], 20);
    EXPECT_EQ(vec2[2], 30);
}

TEST(VectorNonMemberTest, ThreeWayComparison)
{
    // equal
    Vector<int> v1{1, 2, 3};
    Vector<int> v2{1, 2, 3};
    EXPECT_EQ(v1 <=> v2, std::strong_ordering::equal);
    EXPECT_TRUE(v1 == v2);
    EXPECT_FALSE(v1 != v2);
    EXPECT_TRUE(v1 <= v2);
    EXPECT_TRUE(v1 >= v2);

    // less than
    Vector<int> v3{1, 2, 3};
    Vector<int> v4{1, 2, 4};
    EXPECT_EQ(v3 <=> v4, std::strong_ordering::less);
    EXPECT_TRUE(v3 < v4);
    EXPECT_TRUE(v3 <= v4);
    EXPECT_TRUE(v4 > v3);
    EXPECT_TRUE(v4 >= v3);
    EXPECT_TRUE(v3 != v4);

    // greater than
    Vector<int> v5{2, 0, 0};
    Vector<int> v6{1, 9, 9};
    EXPECT_EQ(v5 <=> v6, std::strong_ordering::greater);
    EXPECT_TRUE(v5 > v6);
    EXPECT_TRUE(v6 < v5);

    // diff sizes
    Vector<int> v7{1, 2};
    Vector<int> v8{1, 2, 3};
    EXPECT_EQ(v7 <=> v8, std::strong_ordering::less);
    EXPECT_TRUE(v7 < v8);
    EXPECT_EQ(v8 <=> v7, std::strong_ordering::greater);
    EXPECT_TRUE(v8 > v7);

    // empty test
    Vector<int> v_empty{};
    Vector<int> v_non_empty{1};
    Vector<int> v_empty2{};
    EXPECT_EQ(v_empty <=> v_non_empty, std::strong_ordering::less);
    EXPECT_TRUE(v_empty < v_non_empty);
    EXPECT_EQ(v_empty <=> v_empty2, std::strong_ordering::equal);
    EXPECT_TRUE(v_empty == v_empty2);
}