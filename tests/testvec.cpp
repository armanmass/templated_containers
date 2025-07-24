#include <gtest/gtest.h>
#include "vector.hpp"

#include <string>

class VectorTest : public ::testing::Test 
{
protected:  
    void SetUp() override 
    { }

    void TearDown() override 
    { }

    Vector<int> intVector;
    Vector<std::string> stringVector;
};