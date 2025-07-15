#include <gtest/gtest.h>
#include "hashtable.hpp"

#include <string>
#include <sstream>

class HashTableTest : public ::testing::Test {
protected:  
    void SetUp() override {
        intTable = new HashTable<int, string>();
        stringTable = new HashTable<string, int>();
    }

    void TearDown() override {
        delete intTable;
        delete stringTable;
    }

    HashTable<int, string>* intTable;
    HashTable<string, int>* stringTable;
};

class HashTableParamTest : public ::testing::TestWithParam<std::pair<int, std::string>> {
protected:
    HashTable<int, string> table;
};

TEST_F(HashTableTest, Init) {
    EXPECT_TRUE(intTable->empty());
    EXPECT_EQ(intTable->size(), 0);
}

TEST_F(HashTableTest, Insertion) {
    intTable->insert(1, "test");

    EXPECT_FALSE(intTable->empty());
    EXPECT_EQ(intTable->size(),1);

    intTable->insert(2, "test2");
    intTable->insert(3, "test3");
    intTable->insert(1, "test1");

    EXPECT_EQ(intTable->get(1), "test1");

    EXPECT_EQ(intTable->size(), 3);
    EXPECT_FALSE(intTable->empty());
    for(int i = 1; i <= 3; i++){
        EXPECT_TRUE(intTable->contains(i));
    }

    for(int i = 4; i <= 6; i++){
        EXPECT_FALSE(intTable->contains(i));
        EXPECT_THROW(intTable->get(i), std::out_of_range);
    }
}

int main(int argc, char** argv){
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}