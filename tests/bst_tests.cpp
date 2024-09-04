#include <gtest/gtest.h>
#include <lib/STL_BST/BinaryTree.hpp>

TEST (BST_TEST, DefaultConstructorTest) {
    BinaryTree<int> asd;
    EXPECT_TRUE(asd.empty());
}

TEST (BST_TEST, InitListConstructorTest) {
    BinaryTree<int> asd {1,2,3,4};
    EXPECT_TRUE(asd.empty());
}

