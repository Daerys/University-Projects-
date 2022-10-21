#pragma once

#include <vector>

class SplayTree
{
private:
    class Node
    {
    public:
        Node * left = nullptr;
        Node * right = nullptr;
        Node * parent = nullptr;
        int value;

        Node(int val)
            : value(val)
        {
        }

        ~Node()
        {
            delete left;
            delete right;
        }
    };

public:
    bool contains(int value) const;
    bool insert(int value);
    bool remove(int value);

    std::size_t size() const;
    bool empty() const;

    std::vector<int> values() const;

    ~SplayTree();

private:
    mutable Node * root = nullptr;
    std::size_t tree_size = 0;

    void splay(Node * v) const;
    void rotate(Node * v) const;
    Node * search_closest_node_by_val(int value) const;
    void inorder(Node * node, std::vector<int> & result) const;
};
