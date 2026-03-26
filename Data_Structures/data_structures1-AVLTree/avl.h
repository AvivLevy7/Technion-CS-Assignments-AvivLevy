
#ifndef DATA_STRUCTURE_1_AVL_H
#define DATA_STRUCTURE_1_AVL_H

#include <iostream>
#include <memory>
#include "horse.h"

using namespace std;

// Template class representing a node in the AVL tree
template <typename T>
class AVLNode {
public:
    shared_ptr<T> data; // The value of the node (using shared_ptr)
    shared_ptr<AVLNode<T>> left;  // Pointer to the left child
    shared_ptr<AVLNode<T>> right; // Pointer to the right child
    int height; // Height of the node in the tree

    // Constructor to initialize a node with a given data
    AVLNode(shared_ptr<T> d) : data(d), left(nullptr), right(nullptr), height(0) {}
};

// Template class representing the AVL tree
template <typename T>
class AVLTree {
private:
    // Pointer to the root of the tree
    shared_ptr<AVLNode<T>> root;

    // Custom function to get the maximum of two integers
    static int max1(int a, int b) {
        return (a > b) ? a : b;
    }

    // Function to get the height of a node
    int height(shared_ptr<AVLNode<T>> node) {
        if (node == nullptr)
            return -1; // Return -1 for null nodes to follow height convention
        return node->height;
    }

    // Function to get the balance factor of a node
    int balanceFactor(shared_ptr<AVLNode<T>> node) {
        if (node == nullptr) {
            return 0;
        }
        return height(node->left) - height(node->right);
    }

    // Function to perform a right rotation on a subtree
    shared_ptr<AVLNode<T>> rightRotate(shared_ptr<AVLNode<T>> y) {
        shared_ptr<AVLNode<T>> x = y->left;
        shared_ptr<AVLNode<T>> T2 = x->right;

        // Perform rotation
        x->right = y;
        y->left = T2;

        // Update heights
        y->height = max(height(y->left), height(y->right)) + 1;
        x->height = max(height(x->left), height(x->right)) + 1;

        // Return new root
        return x;
    }

    // Function to perform a left rotation on a subtree
    shared_ptr<AVLNode<T>> leftRotate(shared_ptr<AVLNode<T>> x) {
        shared_ptr<AVLNode<T>> y = x->right;
        shared_ptr<AVLNode<T>> T2 = y->left;

        y->left = x;
        x->right = T2;

        // Update heights
        x->height = max1(height(x->left), height(x->right)) + 1;
        y->height = max1(height(y->left), height(y->right)) + 1;

        // Return new root
        return y;
    }

    // Function to insert a new data into the subtree rooted with node
    shared_ptr<AVLNode<T>> insert(shared_ptr<AVLNode<T>> node, shared_ptr<T> data) {
        // Perform the normal BST insertion
        if (node == nullptr)
            return make_shared<AVLNode<T>>(data); // Create a new node if tree is empty

        // Compare the data with the node's data using the shared_ptr comparison operators
        if (data < node->data) {  // Use operator< for shared_ptr<T>
            node->left = insert(node->left, data); // Insert in the left subtree
        } else if (data > node->data) {  // Use operator> for shared_ptr<T>
            node->right = insert(node->right, data); // Insert in the right subtree
        } else {
            return node; // Data is already in the tree (no duplicates)
        }

        // Update height of the current node
        node->height = 1 + max(height(node->left), height(node->right));

        // Get the balance factor of the current node
        int balance = balanceFactor(node);

        // If the node becomes unbalanced, there are 4 cases to handle

        // Left Left Case
        if (balance > 1 && data < node->left->data) {
            return rightRotate(node); // Right rotate to balance
        }

        // Right Right Case
        if (balance < -1 && data > node->right->data) {
            return leftRotate(node); // Left rotate to balance
        }

        // Left Right Case
        if (balance > 1 && data > node->left->data) {
            node->left = leftRotate(node->left); // Left rotate the left subtree
            return rightRotate(node); // Right rotate to balance
        }

        // Right Left Case
        if (balance < -1 && data < node->right->data) {
            node->right = rightRotate(node->right); // Right rotate the right subtree
            return leftRotate(node); // Left rotate to balance
        }

        // Return the (unchanged) node pointer
        return node;
    }

    // Function to find the node with the minimum data value
    shared_ptr<AVLNode<T>> minValueNode(shared_ptr<AVLNode<T>> node) {
        shared_ptr<AVLNode<T>> current = node;
        while (current->left != nullptr)
            current = current->left;
        return current;
    }

    // Function to delete a data from the subtree rooted with root
    shared_ptr<AVLNode<T>> deleteNode(shared_ptr<AVLNode<T>> root, int id) {
        if (root == nullptr)
            return root;

        // Traverse the tree to find the node to delete
        if (root->data->getId() < id)
            root->right = deleteNode(root->right, id);
        else if (root->data->getId() > id)
            root->left = deleteNode(root->left, id);
        else {
            // Node with one child or no child
            if (root->left == nullptr || root->right == nullptr) {
                shared_ptr<AVLNode<T>> temp = root->left ? root->left : root->right;

                if (temp == nullptr) {
                    // No child, just delete the current node
                    root = nullptr;  // The shared_ptr will automatically release the memory of the node
                } else {
                    // One child, replace the node with the child
                    root = temp; // The shared_ptr of temp will now manage the node, and root is automatically released
                }
            } else {
                // Node with two children: Get the inorder successor
                shared_ptr<AVLNode<T>> temp = minValueNode(root->right);
                root->data = temp->data;  // Copy the inorder successor's data to the current node
                root->right = deleteNode(root->right, temp->data->getId()); // Delete the inorder successor
            }
        }

        if (root == nullptr)
            return root;  // If root is now null, return null

        // Update the height and balance the tree
        root->height = 1 + max(height(root->left), height(root->right));
        int balance = balanceFactor(root);

        // Balance the tree if needed
        if (balance > 1 && balanceFactor(root->left) >= 0)
            return rightRotate(root);
        if (balance > 1 && balanceFactor(root->left) < 0) {
            root->left = leftRotate(root->left);
            return rightRotate(root);
        }
        if (balance < -1 && balanceFactor(root->right) <= 0)
            return leftRotate(root);
        if (balance < -1 && balanceFactor(root->right) > 0) {
            root->right = rightRotate(root->right);
            return leftRotate(root);
        }

        return root;  // Return the (possibly updated) root
    }

    // Function to perform inorder traversal of the tree
    void inorder(shared_ptr<AVLNode<T>> root) {
        if (root != nullptr) {
            inorder(root->left);
            cout << *(root->data) << " ";
            inorder(root->right);
        }
    }

    shared_ptr<AVLNode<T>> search(shared_ptr<AVLNode<T>> root, int id) const {
        if (root == nullptr)
            return nullptr;

        if (root->data && root->data->getId() == id) {
            return root;
        }

        if (id < root->data->getId()) {
            return search(root->left, id);
        }

        return search(root->right, id);
    }

    int countNodesMatching(const shared_ptr<AVLNode<T>>& node, bool (*predicate)(const T&)) const {
        if (!node) {
            return 0;
        }

        int count = predicate(*(node->data)) ? 1 : 0;
        count += countNodesMatching(node->left, predicate);
        count += countNodesMatching(node->right, predicate);

        return count;
    }

    int getSize(shared_ptr<AVLNode<T>> node) const {
        if (node == nullptr) {
            return 0;  // Null node contributes 0 to the size
        }
        // Size is 1 (for the current node) plus the sizes of the left and right subtrees
        return 1 + getSize(node->left) + getSize(node->right);
    }

    bool applyToAllNodes(shared_ptr<AVLNode<T>> node, bool (*func)(T&)) {
        if (node == nullptr) {
            return true;
        }
        // Apply the function to the data of the current node
        if (func(*(node->data))) {
            // Recursively apply to left and right children
            return applyToAllNodes(node->left, func) && applyToAllNodes(node->right, func);
        }
        return false;
    }


public:
    // Constructor to initialize the AVL tree
    AVLTree() : root(nullptr) {}

    // Function to insert a data into the AVL tree
    void insert(shared_ptr<T> data) {
        root = insert(root, data);
    }

    // Function to remove a data from the AVL tree
    void remove(int id) {
        root = deleteNode(root, id);
    }

    // Function to search for a data in the AVL tree
    shared_ptr<AVLNode<T>> search(int id) const {
        return search(root, id);
    }

    int countMatching(bool (*predicate)(const T&)) const {
        if (root != nullptr && root->left == nullptr && root->right == nullptr) {
            return 1;
        }

        return countNodesMatching(root, predicate);
    }

    bool isEmpty() const {
        return root == nullptr;
    }


    int getSize() const {
        return getSize(root);
    }

    bool applyToAllNodes(bool (*func)(T&)) {
        return applyToAllNodes(root, func);
    }
};

#endif //DATA_STRUCTURE_1_AVL_H

