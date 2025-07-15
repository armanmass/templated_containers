#include <gtest/gtest.h>
#include "hashtable.hpp"

#include <string>
#include <utility>

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

class HashTableParamTest : public ::testing::TestWithParam<pair<string,string>> {
protected:
    HashTable<string, string>* table;
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

TEST_F(HashTableTest, Removal){
    intTable->insert(2, "test2");
    EXPECT_EQ(intTable->size(), 1);

    intTable->remove(2);
    EXPECT_EQ(intTable->size(), 0);

    EXPECT_THROW(intTable->remove(2), std::out_of_range);
}

TEST_F(HashTableTest, Types) {
    stringTable->insert("test1",1);
    stringTable->insert("test1",1);
    stringTable->insert("test1",1);
    EXPECT_EQ(stringTable->size(), 1);
    EXPECT_FALSE(stringTable->empty());
    EXPECT_EQ(stringTable->get("test1"), 1);
}

// test p prac
INSTANTIATE_TEST_SUITE_P (
    MultipleInputs,
    HashTableParamTest,
    ::testing::Values(
        make_pair(string("dog"), string("test1")),
        make_pair(string("family"), string("testing")),
        make_pair(string("1"), string("0")),
        make_pair(string("304030923"), string("key value big"))
    )
);

TEST_P(HashTableParamTest, InsertMulti) {
    auto [key,value] = GetParam();
    table->insert(key,value);

    EXPECT_EQ(table->size(),1);
    EXPECT_FALSE(table->empty());
}

// stress test
TEST(HashTableStress, LargeInsert) {
    HashTable<int,int>* t;
    const int BIG = 1000000;
    for(int i = 0; i < BIG; i++){
        t->insert(i,i*2);
    }

    EXPECT_EQ(t->size(), BIG);

    for(int i = 0; i < BIG/2; i++){
        t->remove(i);
    }

    // might be issue if  BIG is odd just for test
    EXPECT_EQ(t->size(), BIG/2);
}



int main(int argc, char** argv){
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}