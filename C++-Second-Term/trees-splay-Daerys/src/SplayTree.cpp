#include "SplayTree.h"

void SplayTree::rotate(Node * v) const
{
    Node * dad = v->parent;
    Node * granny = dad->parent;
    bool flag = dad->right == v;
    Node *& v_child = flag ? v->left : v->right; // ok *& is godlike
    (flag ? dad->right : dad->left) = v_child;
    if (v_child != nullptr) {
        v_child->parent = dad;
    }
    v_child = dad;
    dad->parent = v;
    v->parent = granny;
    if (granny == nullptr) {
        root = v;
    }
    else if (granny->left == dad) {
        granny->left = v;
    }
    else {
        granny->right = v;
    }
}

void SplayTree::splay(Node * v) const
{
    while (v->parent != nullptr) { // equals that v != root
        if (v->parent->parent != nullptr) {
            Node * dad = v->parent;
            Node * granny = dad->parent;
            if ((granny->left == dad && dad->left == v) || (granny->right == dad && dad->right == v)) {
                rotate(v->parent);
            }
            else {
                rotate(v);
            }
        }
        rotate(v);
    }
}

bool SplayTree::insert(int value)
{
    tree_size++;
    if (root == nullptr) {
        root = new Node(value);
        return true;
    }
    Node * curr_node = search_closest_node_by_val(value); // I don't need to splay node, so no contains needed
    if (curr_node->value == value) {
        tree_size--;
        return false;
    }
    Node *& curr_node_child = (value < curr_node->value) ? curr_node->left : curr_node->right;
    curr_node_child = new Node(value);
    curr_node_child->parent = curr_node;
    splay(curr_node_child);
    return true;
}

bool SplayTree::remove(int value)
{
    if (!contains(value)) {
        return false;
    }
    Node * curr_node = root;
    if (curr_node->left == nullptr) {
        root = curr_node->right;
    }
    else if (curr_node->right == nullptr) {
        root = curr_node->left;
    }
    else {
        Node * left = curr_node->left;
        Node * right = curr_node->right;
        left->parent = right->parent = nullptr;
        while (left->right != nullptr) {
            left = left->right;
        }
        splay(left);
        left->right = right;
        right->parent = left;
        root = left;
    }
    curr_node->left = curr_node->right = nullptr;
    delete curr_node;
    if (root != nullptr) {
        root->parent = nullptr;
    }
    tree_size--;
    return true;
}

SplayTree::Node * SplayTree::search_closest_node_by_val(int value) const
{
    Node * curr_node = root;
    Node * prev_node = root;
    while (curr_node != nullptr && curr_node->value != value) {
        prev_node = curr_node;
        curr_node = (value < curr_node->value) ? curr_node->left : curr_node->right;
    }
    return (curr_node == nullptr) ? prev_node : curr_node;
}

bool SplayTree::contains(int value) const
{
    Node * curr_node = search_closest_node_by_val(value);
    if (curr_node == nullptr) {
        return false;
    }
    splay(curr_node);
    return curr_node->value == value;
}

bool SplayTree::empty() const
{
    return root == nullptr;
}

void SplayTree::inorder(Node * node, std::vector<int> & result) const
{
    if (node != nullptr) {
        inorder(node->left, result);
        result.push_back(node->value);
        inorder(node->right, result);
    }
}

std::vector<int> SplayTree::values() const
{
    std::vector<int> ans;
    ans.reserve(tree_size);
    inorder(root, ans);
    return ans;
}

std::size_t SplayTree::size() const
{
    return tree_size;
}
SplayTree::~SplayTree()
{
    delete root;
}
