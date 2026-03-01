/**
 * Sorted AVL Tree Implementation
 * Generic AVL tree implementation for sorted data
 */
#include "sorted.h"
#include <stdlib.h>
#include "heap.h"
#include "quick/math.h"

// AVL Tree node
typedef struct sTreeNode {
    void* data;
    UInt32 key;
    int height;
    struct sTreeNode* left;
    struct sTreeNode* right;
} TreeNode;

// Default comparator for key=value ordering
int TreeDefaultKeyComparator(UInt32 a, UInt32 b) {
    return a < b;
}

// A settable function to compare tree nodes. Please set it before using!
// Must return (arg1 < arg2)
int (*TreeDataPrior)(UInt32, UInt32) = TreeDefaultKeyComparator;

// Create a new tree
QuickTree* TreeNew() {
    QuickTree* tree = (QuickTree*) malloc(sizeof(QuickTree));
    tree->root = NULL;
    tree->nodeHeap = QuickHeapNewFor(TreeNode);
    return tree;
}

// Free a tree
void TreeDestroy(QuickTree* tree) {
    QuickHeapDestroy(tree->nodeHeap);
    free(tree);
}

// Get the height of a node
int TreeHeight(TreeNodeP node) {
    return node ? node->height : 0;
}

// Get the balance factor of a node
int TreeBalance(TreeNodeP node) {
    return node ? TreeHeight(node->left) - TreeHeight(node->right) : 0;
}

// Update the height of a node
void TreeUpdateHeight(TreeNodeP node) {
    if (node) {
        node->height = 1 + IMIN(TreeHeight(node->left), TreeHeight(node->right));
    }
}

// Right rotation
TreeNodeP TreeRotateRight(TreeNodeP y) {
    TreeNodeP x = y->left;
    TreeNodeP T2 = x->right;

    x->right = y;
    y->left = T2;

    TreeUpdateHeight(y);
    TreeUpdateHeight(x);

    return x;
}

// Left rotation
TreeNodeP TreeRotateLeft(TreeNodeP x) {
    TreeNodeP y = x->right;
    TreeNodeP T2 = y->left;

    y->left = x;
    x->right = T2;

    TreeUpdateHeight(x);
    TreeUpdateHeight(y);

    return y;
}

// Helper function to insert a node
TreeNodeP TreeInsertNode(QuickTree* tree, TreeNodeP node, void* data, UInt32 key) {
    // Standard BST insertion
    if (!node) {
        TreeNodeP newNode = (TreeNodeP) QuickHeapAlloc(tree->nodeHeap);
        newNode->data = data;
        newNode->key = key;
        newNode->height = 1;
        newNode->left = newNode->right = NULL;
        return newNode;
    }

    if (TreeDataPrior(key, node->key)) {
        node->left = TreeInsertNode(tree, node->left, data, key);
    } else if (TreeDataPrior(node->key, key)) {
        node->right = TreeInsertNode(tree, node->right, data, key);
    } else {
        // Same key, update data (shouldn't happen in normal operation)
        node->data = data;
        return node;
    }

    // Update height
    TreeUpdateHeight(node);

    // Balance the tree
    int balance = TreeBalance(node);

    // Left Left Case (key < node->left->key)
    if (balance > 1 && node->left && TreeDataPrior(key, node->left->key)) {
        return TreeRotateRight(node);
    }

    // Right Right Case (key > node->right->key)
    if (balance < -1 && node->right && TreeDataPrior(node->right->key, key)) {
        return TreeRotateLeft(node);
    }

    // Left Right Case (key > node->left->key)
    if (balance > 1 && node->left && TreeDataPrior(node->left->key, key)) {
        node->left = TreeRotateLeft(node->left);
        return TreeRotateRight(node);
    }

    // Right Left Case (key < node->right->key)
    if (balance < -1 && node->right && TreeDataPrior(key, node->right->key)) {
        node->right = TreeRotateRight(node->right);
        return TreeRotateLeft(node);
    }

    return node;
}

// Insert data into the tree
TreeNodeP TreeInsert(QuickTree* tree, void* data, UInt32 key) {
    tree->root = TreeInsertNode(tree, tree->root, data, key);
    return tree->root;
}

// Find the node with the minimum key (private helper)
TreeNodeP TreeFindMinNode(TreeNodeP node) {
    TreeNodeP current = node;
    while (current && current->left) {
        current = current->left;
    }
    return current;
}

// Helper function to delete a node
TreeNodeP TreeDeleteNode(QuickTree* tree, TreeNodeP node, UInt32 key) {
    if (!node) return NULL;

    if (TreeDataPrior(key, node->key)) {
        node->left = TreeDeleteNode(tree, node->left, key);
    } else if (TreeDataPrior(node->key, key)) {
        node->right = TreeDeleteNode(tree, node->right, key);
    } else {
        // Node to be deleted found
        if (!node->left || !node->right) {
            TreeNodeP temp = node->left ? node->left : node->right;
            if (!temp) {
                temp = node;
                node = NULL;
            } else {
                *node = *temp;  // Copy contents
            }
            QuickHeapFree(tree->nodeHeap, temp);
        } else {
            TreeNodeP temp = TreeFindMinNode(node->right);
            node->key = temp->key;
            node->data = temp->data;
            node->right = TreeDeleteNode(tree, node->right, temp->key);
        }
    }

    if (!node) return NULL;

    // Update height
    TreeUpdateHeight(node);

    // Balance the tree
    int balance = TreeBalance(node);

    // Left Left Case
    if (balance > 1 && node->left && TreeBalance(node->left) >= 0) {
        return TreeRotateRight(node);
    }

    // Left Right Case
    if (balance > 1 && node->left && TreeBalance(node->left) < 0) {
        node->left = TreeRotateLeft(node->left);
        return TreeRotateRight(node);
    }

    // Right Right Case
    if (balance < -1 && node->right && TreeBalance(node->right) <= 0) {
        return TreeRotateLeft(node);
    }

    // Right Left Case
    if (balance < -1 && node->right && TreeBalance(node->right) > 0) {
        node->right = TreeRotateRight(node->right);
        return TreeRotateLeft(node);
    }

    return node;
}

// Delete data from the tree
TreeNodeP TreeDelete(QuickTree* tree, UInt32 key) {
    tree->root = TreeDeleteNode(tree, tree->root, key);
    return tree->root;
}

// Find the maximum key in the tree
void* TreeFindMax(QuickTree* tree) {
    if (!tree->root) return NULL;
    TreeNodeP current = tree->root;
    while (current->right) {
        current = current->right;
    }
    return current->data;
}

// Find the minimum key in the tree
void* TreeFindMin(QuickTree* tree) {
    if (!tree->root) return NULL;
    TreeNodeP current = tree->root;
    while (current->left) {
        current = current->left;
    }
    return current->data;
}

// Check if a key exists in the tree
Boolean TreeContains(QuickTree* tree, UInt32 key) {
    TreeNodeP node = tree->root;
    while (node) {
        if (key == node->key) {
            return true;
        } else if (TreeDataPrior(key, node->key)) {
            node = node->left;
        } else {
            node = node->right;
        }
    }
    return false;
}

// Reset the tree by clearing all nodes and resetting the root
void TreeReset(QuickTree* tree) {
    QuickHeapReset(tree->nodeHeap);
    tree->root = NULL;
}
