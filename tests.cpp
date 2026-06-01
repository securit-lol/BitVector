#include <gtest/gtest.h>
#include "BitVector/bitvector.h"

TEST(InitialazingContainerTest, CheckInitContainerSize) {
    BoolVector<> _this(30);

    int size = 0;
    for (auto m : _this) {
        ++size;
    }
        
    EXPECT_EQ(size, 30);
}

TEST(InitialazingContainerTest, CheckCopyContainerSize) {
    BoolVector<> _this(30);
    BoolVector<> _this_copy = _this;
    int size = 0;
    for (auto m : _this_copy) {
        ++size;
    }
        
    EXPECT_EQ(size, 30);
}

TEST(InitialazingContainerTest, ConstCheckInitContainerSize) {
    const BoolVector<> _this(30);
    int size = 0;
    for (auto m : _this) {
        ++size;
    }
        
    EXPECT_EQ(size, 30);
}

TEST(InitialazingContainerTest, ConstCheckCopyContainerSize) {
    const BoolVector<> _this(30);
    BoolVector<> _this_copy = _this;
    int size = 0;
    for (auto m : _this_copy) {
        ++size;
    }
        
    EXPECT_EQ(size, 30);
}

TEST(InitialazingContainerTest, ConstConstCheckCopyContainerSize) {
    const BoolVector<> _this(30);
    const BoolVector<> _this_copy = _this;
    int size = 0;
    for (auto m : _this_copy) {
        ++size;
    }
        
    EXPECT_EQ(size, 30);
}

TEST(InitialazingContainerTest, noConstConstCheckCopyContainerSize) {
    BoolVector<> _this(30);
    const BoolVector<> _this_copy = _this;
    int size = 0;
    for (auto m : _this_copy) {
        ++size;
    }
        
    EXPECT_EQ(size, 30);
}

TEST(ValueContainerTest, PushBackValueTesting) {
    BoolVector<> _this(0);
    _this.push_back(true);
    _this.push_back(false);

    EXPECT_TRUE(_this[0]);
    EXPECT_FALSE(_this[1]);
}

TEST(ValueContainerTest, CheckEmpty) {
    BoolVector<> _this(0);
    _this.push_back(true);

    EXPECT_FALSE(_this.empty());
    _this.clear();
    EXPECT_TRUE(_this.empty());
}

TEST(ValueContainerTest, CheckPopBack) {
    BoolVector<> _this(0);
    _this.push_back(true);
    _this.push_back(true);
    EXPECT_EQ(_this.size(), 2);
    _this.pop_back();
    EXPECT_EQ(_this.size(), 1);
}

TEST(ValueContainerTest, CheckFrontAndBack) {
    BoolVector<> _this(0);
    _this.push_back(true);
    _this.push_back(false);
    EXPECT_TRUE(_this.front());
    EXPECT_FALSE(_this.back());
}

TEST(ValueContainerTest, PushBackSizeTesting) {
    BoolVector<> _this(0);
    for (int i = 0; i < 10; ++i) {
        _this.push_back(false);
    }
    EXPECT_EQ(_this.size(), 10);
}

TEST(ValueContainerTest, ShringToFitChecking) {
    BoolVector<> _this(0);

    for(int i = 0; i < 16; ++i) {
        _this.push_back(true);
    }
    EXPECT_EQ(_this.size(), 16);
    EXPECT_EQ(_this.capacity(), 16);
    _this.clear();
    EXPECT_EQ(_this.capacity(), 16);
    _this.shrink_to_fit();
    EXPECT_EQ(_this.capacity(), 0);
}

TEST(ValueContainerTest, CheckCapacity) {
    BoolVector<> _this(0);
    auto it_1 = _this.begin();

    for (int i = 0; i < 8; ++i) {
        _this.push_back(true);
    }
    EXPECT_EQ(_this.capacity(), 1 * 8);
    _this.push_back(true);
    EXPECT_EQ(_this.capacity(), 2 * 8);
}

TEST(IteratorContainerTest, CheckIteratorsAddress) {
    BoolVector<> _this(10);
    auto it_1 = _this.begin();
    auto it_2 = _this.begin();

    for (int i = 0; i < 7; ++i) {
        ++it_1;
        EXPECT_EQ(it_1.data_, it_2.data_);
    }
    ++it_1;
    EXPECT_NE(it_1.data_, it_2.data_);
}

TEST(IteratorContainerTest, CheckIteratorsMovingBegin) {
    BoolVector<> _this(0);
    _this.push_back(true);
    _this.push_back(false);
    auto it_1 = _this.begin();

    EXPECT_TRUE(*it_1++);
    EXPECT_FALSE(*it_1--);
    EXPECT_TRUE(*it_1);
}
TEST(IteratorContainerTest, CheckIteratorsMovingEnd) {
    BoolVector<> _this(0);
    _this.push_back(true);
    _this.push_back(false);
    _this.push_back(false);
    auto it_1 = _this.end();

    EXPECT_FALSE(*--it_1);
    EXPECT_FALSE(*--it_1);
    EXPECT_TRUE(*--it_1);
}