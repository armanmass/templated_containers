#include "gtest/gtest.h"
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


TEST(VectorTest, DefaultConstructor) 
{
    Vector<int> v;
    EXPECT_EQ(v.size(), 0);
    EXPECT_EQ(v.capacity(), 0);
    EXPECT_TRUE(v.empty());
}


TEST_F(PopulatedVectorTest, OperatorBracket) 
{
    EXPECT_EQ(vec[0], 10);
    EXPECT_EQ(vec[2], 30);

    vec[1] = 25;
    EXPECT_EQ(vec[1], 25);
}


TEST_F(PopulatedVectorTest, At) 
{
    EXPECT_EQ(vec.at(0), 10);
    EXPECT_EQ(vec.at(3), 40);

    vec.at(1) = 25;
    EXPECT_EQ(vec.at(1), 25);
}

TEST_F(PopulatedVectorTest, AtThrowsOutOfRange) 
{
    Vector<int> v;
    v.push_back(1);

    EXPECT_THROW(v.at(1), std::out_of_range); 
    EXPECT_THROW(v.at(100), std::out_of_range); 
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
    EXPECT_EQ(v.capacity(), 0);
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

    
    v.pop_back();
    EXPECT_EQ(v.size(), 0);
    EXPECT_TRUE(v.empty());
}


TEST_F(PopulatedVectorTest, Clear)
{
    EXPECT_FALSE(vec.empty());
    vec.clear();
    EXPECT_TRUE(vec.empty());
    EXPECT_EQ(vec.size(), 0);
    EXPECT_EQ(vec.capacity(), 8); 
}

TEST(VectorTest, Reserve)
{
    Vector<int> v;
    EXPECT_EQ(v.capacity(), 8);

    v.reserve(100);
    EXPECT_EQ(v.capacity(), 100);
    EXPECT_EQ(v.size(), 0);

    
    v.reserve(50);
    EXPECT_EQ(v.capacity(), 100);
}

TEST_F(PopulatedVectorTest, ShrinkToFit)
{
    
    vec.reserve(100);
    EXPECT_EQ(vec.capacity(), 100);
    EXPECT_EQ(vec.size(), 4);

    vec.shrink_to_fit();
    EXPECT_EQ(vec.capacity(), 4);
    EXPECT_EQ(vec.size(), 4);

    
    EXPECT_EQ(vec[3], 40);
}

TEST(VectorTest, ShrinkToFitOnEmptyThenPush)
{
    Vector<int> v;
    v.shrink_to_fit(); 
    
    EXPECT_EQ(v.capacity(), 0); 
}




TEST(VectorTest, CopyAssignmentOperator)
{
    Vector<int> v1;
    v1.push_back(1);
    v1.push_back(2);
    
    Vector<int> v2;
    v2.push_back(99);

    v2 = v1;

    EXPECT_EQ(v1.size(), v2.size());
    EXPECT_EQ(v1.capacity(), v2.capacity());
    
    for (size_t i = 0; i < v1.size(); ++i)
    {
        EXPECT_EQ(v1[i], v2[i]);
    }
    
    v1[0] = 50;
    EXPECT_EQ(v2[0], 1);
}

TEST(VectorTest, CopyAssignmentSelfAssignment)
{
    Vector<int> v1;
    v1.push_back(1);
    v1.push_back(2);
    
    v1 = v1;

    EXPECT_EQ(v1.size(), 2);
    EXPECT_EQ(v1[0], 1);
    EXPECT_EQ(v1[1], 2);
}


TEST(VectorTest, DestructorWorks)
{
    {
        Vector<int> v;
        v.push_back(1);
        v.reserve(100); 
    } 
    
    SUCCEED(); 
}