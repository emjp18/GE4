#pragma once
#include "pch.h"
using namespace std;

namespace CUSTOM_CONTAINER
{
	class Map
	{
    public:
        static class Map* root;

        // Number of elements in the map
        static int cnt;

        // Left child, right child and parent
        Map* left, * right, * par;

        // First is key, second is value
        // and depth is height of the subtree
        // for the given node
        int first, second, depth;
    
        inline ~Map()
        {
            clear();
        }
    private:
         inline void iterate(Map* head = root)
        {
            if (root == 0)
                return;
            if (head->left != 0) {
                iterate(head->left);
            }

          

            if (head->right != 0) {
                iterate(head->right);
            }
        }
    public:
        inline void clear(void)
        {
            while (root != nullptr) {
                erase(root->first);
            }
        }
        inline bool empty(void)
        {
            if (root == 0)
                return true;
            return false;
        }
        inline int count(int first)
        {
            Map* temp = iterator(first);

            // If key is found
            if (temp != nullptr)
                return 1;

            // If key is not found
            return 0;
        }
    private:
        // Returns the previous node in
    // inorder traversal of the AVL Tree.
        inline Map* inorderPredecessor(Map* head)
        {
            if (head == nullptr)
                return head;
            while (head->right != nullptr) {
                head = head->right;
            }
            return head;
        }

        // Returns the next node in
        // inorder traversal of the AVL Tree.
        inline Map* inorderSuccessor(Map* head)
        {
            if (head == nullptr)
                return head;
            while (head->left != nullptr) {
                head = head->left;
            }
            return head;
        }
    public:
        inline void erase(int first, Map* temp = root)
        {
            Map* prev = 0;
            cnt--;
            while (temp != 0 && temp->first != first) {
                prev = temp;
                if (first < temp->first) {
                    temp = temp->left;
                }
                else if (first > temp->first) {
                    temp = temp->right;
                }
            }
            if (temp == nullptr) {
                cnt++;
                return;
            }
            if (cnt == 0 && temp == root) {
                free(temp);
                root = nullptr;
                return;
            }
            Map* l = inorderPredecessor(temp->left);
            Map* r = inorderSuccessor(temp->right);
            if (l == 0 && r == 0) {
                if (prev == 0) {
                    root = 0;
                }
                else {
                    if (prev->left == temp) {
                        prev->left = 0;
                    }
                    else {
                        prev->right = 0;
                    }
                    free(temp);
                    balance(prev);
                }
                return;
            }

            Map* start;
            if (l != 0) {
                if (l == temp->left) {
                    l->right = temp->right;
                    if (l->right != 0) {
                        l->right->par = l;
                    }
                    start = l;
                }
                else {
                    if (l->left != 0) {
                        l->left->par = l->par;
                    }
                    start = l->par;
                    l->par->right = l->left;
                    l->right = temp->right;
                    l->par = 0;
                    if (l->right != 0) {
                        l->right->par = l;
                    }
                    l->left = temp->left;
                    temp->left->par = l;
                }
                if (prev == 0) {
                    root = l;
                }
                else {
                    if (prev->left == temp) {
                        prev->left = l;
                        l->par = prev;
                    }
                    else {
                        prev->right = l;
                        l->par = prev;
                    }
                    free(temp);
                }
                balance(start);
                return;
            }
            else {
                if (r == temp->right) {
                    r->left = temp->left;
                    if (r->left != 0) {
                        r->left->par = r;
                    }
                    start = r;
                }
                else {
                    if (r->right != 0) {
                        r->right->par = r->par;
                    }
                    start = r->par;
                    r->par->left = r->right;
                    r->left = temp->left;
                    r->par = 0;
                    if (r->left != 0) {
                        r->left->par = r;
                    }
                    r->right = temp->right;
                    temp->right->par = r;
                }
                if (prev == 0) {
                    root = r;
                }
                else {
                    if (prev->right == temp) {
                        prev->right = r;
                        r->par = prev;
                    }
                    else {
                        prev->left = r;
                        r->par = prev;
                    }
                    free(temp);
                }
                balance(start);
                return;
            }
        }
          private:
              inline const Map* iterator(int first) const
        {
            Map* temp = root;
            while (temp != nullptr
                && temp->first != first) {
                if (first < temp->first)
                    temp = temp->left;
                else
                    temp = temp->right;
            }
            return temp;
        }
         public:
             inline const int search(int first) const
        {
            const Map* temp = iterator(first);

            // If element exists with the given key
            // return its value
            if (temp != nullptr)
                return temp->second;

            // If element doesn't exist
            // return default value of 0
            return 0;
        }
             inline void update(int first, int second)
        {
            Map* temp = iterator(first);
            if (temp != nullptr) {
                temp->second = second;
            }
        }
             inline const int operator[](int key) const
        {
            // Search method is also qualified with const
            return search(key);
        }
        private:
            inline Map* iterator(int first)
        {
            Map* temp = root;
            while (temp != nullptr && temp->first != first) {
                if (first < temp->first) {
                    temp = temp->left;
                }
                else {
                    temp = temp->right;
                }
            }
            return temp;
        }
        public:
            inline  Map* find(int first) {
            return iterator(first);
        }
            inline Map* insert(int first)
        {
            // Increase the number of elements
            cnt++;
            Map* newnode = create(first);

            // If empty tree simply create the root
            if (root == nullptr) {
                root = newnode;
                return root;
            }
            Map* temp = root, * prev = nullptr;
            while (temp != nullptr) {
                prev = temp;
                if (first < temp->first)
                    temp = temp->left;
                else if (first > temp->first)
                    temp = temp->right;
                else {
                    free(newnode);

                    // If element already exists
                    // decrease the count
                    cnt--;

                    // If the key is found then it is
                    // returned by reference so that it is
                    // updatable
                    return temp;
                }
            }
            if (first < prev->first)
                prev->left = newnode;
            else
                prev->right = newnode;

            newnode->par = prev;

            // Once inserted Check and balance the tree
            // at every node in path from "newnode" to "root"
            balance(newnode);

            // New object is inserted and returned to
            // initialize in the main during assignment
            return newnode;
        }
        // Utility method to return the
    // "depth" of the subtree at the "node"
            inline int depthf(Map* node)
        {
            if (node == nullptr)

                // If it is null node
                return 0;
            return node->depth;
        }
            inline void balance(Map* node)
        {
            while (node != root) {
                int d = node->depth;
                node = node->par;
                if (node->depth < d + 1) {
                    node->depth = d + 1;
                }
                if (node == root
                    && depthf(node->left)
                    - depthf(node->right) > 1) {
                    if (depthf(node->left->left)
            > depthf(node->left->right)) {
                        root = node->left;
                    }
                    else {
                        root = node->left->right;
                    }
                    helper(node);
                    break;
                }
                else if (node == root
                    && depthf(node->left)
                    - depthf(node->right)
                    < -1) {
                    if (depthf(node->right->right)
                    > depthf(node->right->left)) {
                        root = node->right;
                    }
                    else {
                        root = node->right->left;
                    }
                    helper(node);
                    break;
                }
                helper(node);
            }
        }
        private:
            inline void helper(Map* node)
        {
            // If left skewed
            if (depthf(node->left)
                - depthf(node->right) > 1) {

                // If "depth" of left subtree of
                // left child of "node" is
                // greater than right
                // subtree of left child of "node"
                if (depthf(node->left->left)
                > depthf(node->left->right)) {
                    node->depth
                        = max(depthf(node->right) + 1,
                            depthf(node->left->right) + 1);
                    node->left->depth
                        = max(depthf(node->left->left) + 1,
                            depthf(node) + 1);
                    right_rotation(node);
                }

                // If "depth" of right subtree
                // of left child of "node" is
                // greater than
                // left subtree of left child
                else {
                    node->left->depth = max(
                        depthf(node->left->left) + 1,
                        depthf(node->left->right->left)
                        + 1);
                    node->depth
                        = max(depthf(node->right) + 1,
                            depthf(node->left->right->right) + 1);
                    node->left->right->depth
                        = max(depthf(node) + 1,
                            depthf(node->left) + 1);
                    left_rotation(node->left);
                    right_rotation(node);
                }
            }

            // If right skewed
            else if (depthf(node->left)
                - depthf(node->right) < -1) {

                // If "depth" of right subtree of right
                // child of "node" is greater than
                // left subtree of right child
                if (depthf(node->right->right)
                > depthf(node->right->left)) {
                    node->depth
                        = max(depthf(node->left) + 1,
                            depthf(node->right->left) + 1);
                    node->right->depth
                        = max(depthf(node->right->right) + 1,
                            depthf(node) + 1);
                    left_rotation(node);
                }

                // If "depth" of left subtree
                // of right child of "node" is
                // greater than that of right
                // subtree of right child of "node"
                else {
                    node->right->depth = max(
                        depthf(node->right->right) + 1,
                        depthf(node->right->left->right) + 1);
                    node->depth = max(
                        depthf(node->left) + 1,
                        depthf(node->right->left->left) + 1);
                    node->right->left->depth
                        = max(depthf(node) + 1,
                            depthf(node->right) + 1);
                    right_rotation(node->right);
                    left_rotation(node);
                }
            }
        }
        private:
            inline Map* create(int first)
        {
            Map* newnode = (Map*)malloc(sizeof(Map));
            newnode->first = first;
            newnode->second = 0;
            newnode->left = nullptr;
            newnode->right = nullptr;
            newnode->par = nullptr;

            // Depth of a newnode shall be 1
            // and not zero to differentiate
            // between no child (which returns
            // nullptr) and having child(returns 1)
            newnode->depth = 1;
            return newnode;
        }
            inline void right_rotation(Map* x)
        {
            Map* y = x->left;
            x->left = y->right;
            if (y->right != nullptr) {
                y->right->par = x;
            }
            if (x->par != nullptr && x->par->right == x) {
                x->par->right = y;
            }
            else if (x->par != nullptr && x->par->left == x) {
                x->par->left = y;
            }
            y->par = x->par;
            y->right = x;
            x->par = y;
        }
            inline void left_rotation(Map* x)
        {
            Map* y = x->right;
            x->right = y->left;
            if (y->left != nullptr) {
                y->left->par = x;
            }
            if (x->par != nullptr && x->par->left == x) {
                x->par->left = y;
            }
            else if (x->par != nullptr && x->par->right == x) {
                x->par->right = y;
            }
            y->par = x->par;
            y->left = x;
            x->par = y;
        }
        public:
            inline int& operator[](int key) {
            return insert(key)->second;
        }
        
            inline void insert(int first, int second)
        {
            Map* temp = iterator(first);

            // If element doesnot exist already
            if (temp == nullptr)
                insert(first)->second = second;

            // If element exists already update it
            else
                temp->second = second;
        }
	};
    
}


