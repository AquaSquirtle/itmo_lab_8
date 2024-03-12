#pragma once
#include "iostream"
#include "optional"
#include "functional"


enum class Tag {
    PreOrder,
    InOrder,
    PostOrder
};

template <typename T, Tag Order = Tag::PreOrder, typename Allocator = std::allocator<T>, typename Compare = std::less<T>>
class BinaryTree {
public:
    using value_type = T;
    using key_compare = Compare;
    using value_compare = std::function<bool(const value_type&, const value_type&)>;

    BinaryTree(const Allocator& alloc, const Compare& compare = Compare())
    : alloc_(std::allocator_traits<decltype(alloc)>::template rebind_alloc<Node>)
    , compare_(compare)
    , value_compare_(compare)
    {};

    BinaryTree(const Compare& compare = Compare())
    : compare_(compare)
    , value_compare_(compare)
    {};

    BinaryTree(const BinaryTree &other) {
        if (operator!=(other)) {
            if (root_ != nullptr) {
                DeallocateNode(end_);
                DeallocateAllNodes(root_);

            }
            end_ = nullptr;
            begin_ = nullptr;
            compare_ = other.compare_;
            alloc_ = other.alloc_;
            root_ = CopyTree(other.root_, nullptr, other.end_);
            if (Order == Tag::PostOrder && root_ != nullptr) {
                root_ = CreateEndPost(root_);
            }
        }
    };

    BinaryTree(const BinaryTree &other, const Allocator& alloc)
    : alloc_(std::allocator_traits<decltype(alloc)>::template rebind_alloc<Node>)
    {
        if (operator!=(other)) {
            if (root_ != nullptr) {
                DeallocateNode(end_);
                DeallocateAllNodes(root_);

            }
            end_ = nullptr;
            begin_ = nullptr;
            compare_ = other.compare_;
            alloc_ = other.alloc_;
            root_ = CopyTree(other.root_, nullptr, other.end_);
            if (Order == Tag::PostOrder && root_ != nullptr) {
                root_ = CreateEndPost(root_);
            }
        }
    };

    template<typename InputIterator>
    BinaryTree(InputIterator i, InputIterator j, const Compare& compare = Compare())
    : compare_(compare)
    , value_compare_(compare)
    {
        if constexpr (std::is_same<typename std::iterator_traits<InputIterator>::value_type, T>::value) {
            for (; i != j; ++i) {
                Insert(*i);
            }
        }
    }

    BinaryTree(std::initializer_list<T> il, const Compare& compare = Compare())
    : BinaryTree(il.begin(), il.end(), compare)
    {}

    ~BinaryTree() {
        DeallocateNode(end_);
        DeallocateAllNodes(root_);
    };

    BinaryTree& operator=(const BinaryTree &other) {
        if (operator!=(other)) {
            if (root_ != nullptr) {
                DeallocateNode(end_);
                DeallocateAllNodes(root_);

            }
            end_ = nullptr;
            begin_ = nullptr;
            compare_ = other.compare_;
            alloc_ = other.alloc_;
            root_ = CopyTree(other.root_, nullptr, other.end_);
            if (Order == Tag::PostOrder && root_ != nullptr) {
                root_ = CreateEndPost(root_);
            }
            return *this;
        }


    };

private: //переменные
    struct Node {
        Node* left = nullptr;
        Node* right = nullptr;
        Node* parent = nullptr;
        T value;

        Node(const T &value_, Node *parent_, Node* left_ = nullptr, Node* right_ = nullptr)
        : value(value_)
        , parent(parent_)
        , left(left_)
        , right(right_) {};
        Node(Node* other)
        : left(other->left)
        , right(other->right)
        , parent(other->parent)
        , value(other->value) {}
        Node() = default;
    };

    Node* end_ = nullptr;
    Node* root_ = nullptr;
    Node* begin_ = nullptr;


    Compare compare_;
    value_compare value_compare_ = compare_;
    std::allocator_traits<Allocator>::template rebind_alloc<Node> alloc_;

public: //Итераторы
    template<bool IsConstPolicy = false>
    struct PreOrderPolicy {
        using pointer = std::conditional_t<IsConstPolicy, const Node*, Node*>;
        static pointer increment (pointer ptr){
            if (ptr->left != nullptr) {
                ptr = ptr->left;
            } else if (ptr->right != nullptr) {
                ptr = ptr->right;
            } else {
                while (ptr->parent != nullptr) {
                    pointer temp = ptr->parent;
                    if (temp->right != nullptr && ptr == temp->left) {
                        ptr = temp->right;
                        return ptr;
                    }
                    ptr = temp;
                }
                ptr = nullptr;
            }
            return ptr;
        }
        static pointer decrement(pointer ptr){
            if (ptr->parent == nullptr) {
                ptr = nullptr;
            } else if (ptr->parent->left != nullptr && ptr->parent->left != ptr ) {
                pointer temp = ptr->parent->left;
                while (temp->right != nullptr || temp->left != nullptr) {
                    if (temp->right != nullptr) {
                        temp = temp->right;
                    } else if (temp->left != nullptr) {
                        temp = temp->left;
                    }
                }
                ptr = temp;
            } else {
                ptr = ptr->parent;
            }
            return ptr;
        }
    };
    
    template<bool IsConstPolicy = false>
    struct InOrderPolicy {
        using pointer = std::conditional_t<IsConstPolicy, const Node*, Node*>;
        static pointer increment (pointer ptr){
            if (ptr->right != nullptr) {
                pointer temp = ptr->right;
                while (temp->left != nullptr) {
                    temp = temp->left;
                }
                ptr = temp;
            } else {
                pointer temp1 = ptr->parent;
                pointer temp2 = ptr;
                while (temp1 != nullptr && temp2 == temp1->right) {
                    temp2 = temp1;
                    temp1 = temp2->parent;
                }
                ptr = temp1;
            }
            return ptr;
        }
        static pointer decrement(pointer ptr){
            if (ptr->left != nullptr) {
                pointer temp = ptr->left;
                while (temp->right != nullptr) {
                    temp = temp->right;
                }
                ptr = temp;
            } else {
                pointer temp1 = ptr->parent;
                pointer temp2 = ptr;
                while (temp1 != nullptr && temp2 == temp1->left) {
                    temp2 = temp1;
                    temp1 = temp2->parent;
                }
                ptr = temp1;
            }
            return ptr;
        }
    };
    
    template<bool IsConstPolicy = false>
    struct PostOrderPolicy {
        using pointer = std::conditional_t<IsConstPolicy, const Node*, Node*>;
        static pointer increment (pointer ptr){
            if (ptr->parent == nullptr) {
                ptr = nullptr;
            } else if (ptr == ptr->parent->right) {
                ptr = ptr->parent;
            } else {
                if (ptr->parent->right != nullptr) {
                    pointer temp = ptr->parent->right;
                    while (temp->left != nullptr || temp->right != nullptr) {
                        if (temp->left != nullptr) {
                            temp = temp->left;
                        } else {
                            temp = temp->right;
                        }
                    }
                    ptr = temp;
                } else {
                    ptr = ptr->parent;
                }
            }
            return ptr;
        }
        static pointer decrement(pointer ptr){
            if (ptr->right != nullptr) {
                ptr = ptr->right;
            } else if (ptr->left != nullptr) {
                ptr = ptr->left;
            } else if (ptr->parent != nullptr) {
                if (ptr->parent->right == ptr) {
                    if (ptr->parent->left != nullptr) {
                        ptr = ptr->parent->left;
                        return ptr;
                    }
                } else {
                    pointer temp = ptr->parent;
                    if (temp->parent != nullptr && temp->parent->left != nullptr) {
                        ptr = temp->parent->left;
                        return ptr;
                    }
                }
                ptr = ptr->parent;
            }
            return ptr;
        }
    };

    template<bool IsConst = false>
    class Iterator {
    public:

        using pointer = std::conditional_t<IsConst, const Node*, Node*>;
        using reference = std::conditional_t<IsConst, const T&, T&>;

        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::bidirectional_iterator_tag;

        Iterator(pointer ptr) : ptr_(ptr) {};
        Iterator(const Iterator&) = default;
        Iterator& operator=(const Iterator&) = default;
        bool operator==(const Iterator& other) {
            return other.ptr_ == this->ptr_;
        }
        bool operator!=(const Iterator& other) {
            return other.ptr_ != this->ptr_;
        }
        friend bool operator==(const Iterator& lhs, const Iterator& rhs) {
            return lhs.ptr_ == rhs.ptr_;
        }
        friend bool operator!=(const Iterator& lhs, const Iterator& rhs) {
            return lhs.ptr_ != rhs.ptr_;
        }
        reference operator*() const {
            return ptr_->value;
        }
        pointer operator->() const {
            return ptr_;
        }
        Iterator& operator++() {
            if (Order == Tag::PreOrder) {
                ptr_ = PreOrderPolicy<IsConst>::increment(ptr_);
            }
            else if (Order == Tag::InOrder) {
                ptr_ = InOrderPolicy<IsConst>::increment(ptr_);
            }
            else if (Order == Tag::PostOrder) {
                ptr_ = PostOrderPolicy<IsConst>::increment(ptr_);
            }
            return *this;
        }
        Iterator operator++(int) {
            auto temp = *this;
            ++(*this);
            return temp;
        }
        Iterator& operator--() {
            if (Order == Tag::PreOrder) {
                ptr_ = PreOrderPolicy<IsConst>::decrement(ptr_);
            }
            else if (Order == Tag::InOrder) {
                ptr_ = InOrderPolicy<IsConst>::decrement(ptr_);
            }
            else if (Order == Tag::PostOrder) {
                ptr_ = PostOrderPolicy<IsConst>::decrement(ptr_);
            }
            return *this;
        }
        Iterator operator--(int) {
            auto temp = *this;
            --(*this);
            return temp;
        }
    private:
        pointer ptr_;
    };


    using iterator = Iterator<false>;
    using const_iterator = Iterator<true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

public: //Container Requirements
    iterator begin() const{
        return iterator(begin_);
    };

    iterator end() const {
        return iterator(end_);
    };

    const_iterator cend() const {
        return const_iterator(end_);
    };

    const_iterator cbegin() const {
        return const_iterator(begin_);
    };

    size_t size() {
        return std::distance(begin(), end());
    };

    size_t max_size() {
        return SIZE_MAX;
    };

    bool empty() {
        return begin() == end();
    };

    bool operator==(const BinaryTree& rhs) {
        return std::equal(begin(), end(), rhs.begin(), rhs.end());
    };

    bool operator!=(const BinaryTree& rhs) {
        return !operator==(rhs);
    };

public: //Reversible Container Requirements
    reverse_iterator rbegin() const {
        return reverse_iterator(end());
    }

    reverse_iterator rend() const {
        return reverse_iterator(begin());
    }

    const_reverse_iterator crbegin() const {
        return const_reverse_iterator(cend());
    }

    const_reverse_iterator crend() const {
        return const_reverse_iterator (cbegin());
    }


public: // Allocator Aware Container
    decltype(alloc_) get_allocator() {
        return alloc_;
    }

public: // Associative Container
    Compare key_comp() {
        return compare_;
    }

    value_compare value_comp() {
        return value_compare_;
    }

    template<typename InputIterator>
    void insert(InputIterator i, InputIterator j) {
        if constexpr (std::is_same<typename std::iterator_traits<InputIterator>::value_type, T>::value) {
            for (; i != j; ++i) {
                Insert(*i);
            }
        }
    }

    void insert(std::initializer_list<T> il) {
        insert(il.begin(), il.end());
    }

    void insert(const T& key) {
        Insert(key);
    }

    void erase(const T& key) {
        Delete(key);
    }
    template<typename InputIterator>
    void erase(InputIterator i, InputIterator j) {
        if constexpr (std::is_same<typename std::iterator_traits<InputIterator>::value_type, T>::value) {
            for (; i != j; ++i) {
                Delete(*i);
            }
        }
    }

    void clear() {
        erase(begin(), end());
    }

    iterator find(const T& k){
        return std::find(begin(), end(), k);
    }

    bool contains(const T& k) {
        return find(k) != end();
    }



private: //bst

    Node* AllocateNode(const T& key, Node* parent) {
        Node* node = std::allocator_traits<decltype(alloc_)>::allocate(alloc_, 1);
        std::allocator_traits<decltype(alloc_)>::construct(alloc_, node, Node(key, parent));
        DefineBegin(node);
        return node;
    };
    void DeallocateNode(Node* node) {
        if (node != nullptr) {
            if (node->parent != nullptr && node->parent != end_) {
                if (node->parent->right == node) {
                    node->parent->right = nullptr;
                } else {
                    node->parent->left = nullptr;
                }
            }
        }
        std::allocator_traits<decltype(alloc_)>::destroy(alloc_, &node->value);
        std::allocator_traits<decltype(alloc_)>::deallocate(alloc_, node,1);
    };
    void DeallocateAllNodes(Node* node) {
        if (node != nullptr) {
            DeallocateNode(node->left);
            DeallocateNode(node->right);
            DeallocateNode(node);
        }
    }
    Node* Minimum (Node* x) {
        if (x->left == nullptr) {
            return x;
        }
        return Minimum(x->left);
    };
    Node* Insert (Node* node, const T& key, BinaryTree::Node* parent) {
        if (node == nullptr || node == end_) {
            Node* temp = AllocateNode(key, parent);
            node = DefineEnd(node, parent, temp);
        }
        else if (compare_(key, node->value)) {
            node->left = Insert(node->left, key, node);
        }
        else if (compare_(node->value, key)) {
            node->right = Insert(node->right, key, node);
        }
        return node;};
    Node* Delete (Node* node, const T& key) {
        if (node == nullptr) {
            return node;
        }
        if (compare_(key, node->value)) {
            node->left = Delete(node->left, key);
        }
        else if (compare_(node->value, key)) {
            node->right = Delete(node->right, key);
        } else {
            if (node->left == nullptr) {
                Node* temp = node->right;
                if (temp != nullptr) {
                    temp->parent = node->parent;
                }
                DeallocateNode(node);
                return temp;
            }
            else if (node->right == nullptr) {
                Node* temp = node->left;
                if (temp != nullptr) {
                    temp->parent = node->parent;
                }
                DeallocateNode(node);
                return temp;
            }
            Node* temp = Minimum(node->right);
            node->value = temp->value;
            node->right = Delete(node->right, temp->value);
        }
        return node;
    };
    Node* CopyTree(Node* node, Node* parent, Node* end) {
        if (node == nullptr) {
            return nullptr;
        }
        if (node == end && (Order == Tag::PreOrder || Order == Tag::InOrder)) {
            parent = CreateEnd(parent);
            return end_;
        } else if (node != end) {
            Node* temp = AllocateNode(node->value, parent);
            temp->left = CopyTree(node->left, temp, end);
            temp->right = CopyTree(node->right, temp, end);
            return temp;
        } else {
            return nullptr;
        }

    }

    Node* DefineEnd (Node* node, Node* parent, Node* temp) {
        if (Order == Tag::PreOrder) {
            if (end_ == nullptr) {
                node = CreateEnd(temp);
            }
            else if (node == end_) {
                node = RebindEnd(temp, parent);
            } else {
                return temp;
            }
        } else if (Order == Tag::InOrder) {
            if (end_ == nullptr) {
                node = CreateEnd(temp);
            } else if (node == end_) {
                node = RebindEnd(temp, parent);
            } else {
                return temp;
            }
        } else if (Order == Tag::PostOrder) {
            if (end_ == nullptr) {
                node = CreateEndPost(temp);
            } else {
                return temp;
            }
        }
        return node;
    }
    void DefineBegin(Node* node) {
        if (begin_ == nullptr) {
            begin_ = node;
        }
        if (compare_(node->value, begin_->value) && (Order == Tag::InOrder || Order == Tag::PostOrder)) {
            begin_ = node;
        } else if (Order == Tag::PostOrder && node->parent == begin_) {
            begin_ = node;
        }
    }
    Node* CreateEnd(Node* node) {
        end_ = std::allocator_traits<decltype(alloc_)>::allocate(alloc_, 1);
        std::allocator_traits<decltype(alloc_)>::construct(alloc_, end_, Node());
        end_->parent = node;
        node->right = end_;
        return node;
    };
    Node* RebindEnd(Node* node, Node* parent) {
        end_->parent = node;
        node->right = end_;
        parent->right = node;
        return node;
    }
    Node* CreateEndPost(Node* node) {
        end_ = std::allocator_traits<decltype(alloc_)>::allocate(alloc_, 1);
        std::allocator_traits<decltype(alloc_)>::construct(alloc_, end_, Node());
        node->parent = end_;
        end_->right = node;
        return node;
    }



    void Insert(const T& key) {
        root_ = Insert(root_, key, nullptr);
    };
    void Delete (const T& key) {
        root_ = Delete(root_, key);
    };

public:

};



