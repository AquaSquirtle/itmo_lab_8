#include <lib/STL_BST/BinaryTree.hpp>
#include "set"
#include "vector"


template<typename T>
bool asd(T iterator_left, T iterator_right, bool (*predicat)(typename std::iterator_traits<decltype(iterator_left)>::value_type)) {
    for (; iterator_left != iterator_right; ++iterator_left) {
        if (predicat(*iterator_left)) {
            return true;
        }
    }
    return false;
}


bool is_even(int a) {
    return a % 2 == 0;
}

int main () {
    std::vector<int> k = {99,98,97};
    std::cout << asd<decltype(k.begin())>(k.begin(), k.end(), is_even);
    BinaryTree<int, Tag::PostOrder> tree {1,2,3,4,5,6,7,8,9};
    tree.insert(10);
    tree.insert(k.begin(), k.end());
    tree.insert({67,68,69});
    for (auto i: tree) {
        std::cout << i << ' ';
    }
    std::cout << '\n';
    for (auto i: tree) {
        std::cout << i << ' ';
    }
    std::cout << '\n';
    tree.clear();
    std::cout << '\n';
    std::cout << ' ' <<  tree.empty();
    return 0;
}