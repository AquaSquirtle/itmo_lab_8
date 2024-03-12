#include <lib/STL_BST/BinaryTree.hpp>
#include "set"
#include "vector"

int main () {
    BinaryTree<int, Tag::PostOrder> tree {1,2,3,4,5,6,7,8,9};
    tree.insert(10);
    std::vector<int> k = {99,98,97};
    tree.insert(k.begin(), k.end());
    tree.insert({67,68,69});
    for (auto i: tree) {
        std::cout << i << ' ';
    }
    std::cout << '\n';
    for (auto i: tree) {
        std::cout << i << ' ';
    }


    return 0;
}