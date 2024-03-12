#include <lib/STL_BST/BinaryTree.hpp>
#include "set"
#include "vector"

int main () {
    BinaryTree<int, Tag::PostOrder> tree;
    tree.Insert(10);
    tree.Insert(5);
    tree.Insert(16);
    tree.Insert(98);
    tree.Insert(20);
    tree.Insert(100);
    tree.PrintAll();
    std::cout << '\n';
    tree.Delete(11);
    for (auto i: tree) {
        std::cout << i << ' ';
    }



    return 0;
}