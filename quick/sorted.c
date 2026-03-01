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
    int height;
    struct sTreeNode* left;
    struct sTreeNode* right;
} TreeNode;

// Default comparator for data ordering (assumes data is UInt32)
int TreeDefaultDataComparator(void* a, void* b) {
    UInt32 valA = *(UInt32*)a;
    UInt32 valB = *(UInt32*)b;
    if (valA < valB) return -1;
    if (valA > valB) return 1;
    return 0;
}

// A settable function to compare tree data. Please set it before using!
// Must return <0 if a<b, 0 if a==b, >0 if a>b
int (*TreeDataPrior)(void*, void*) = TreeDefaultDataComparator;

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
TreeNodeP TreeInsertNode(QuickTree* tree, TreeNodeP node, void* data) {
    // Standard BST insertion
    if (!node) {
        TreeNodeP newNode = (TreeNodeP) QuickHeapAlloc(tree->nodeHeap);
        newNode->data = data;
        newNode->height = 1;
        newNode->left = newNode->right = NULL;
        return newNode;
    }

    int cmp = TreeDataPrior(data, node->data);
    if (cmp < 0) {
        node->left = TreeInsertNode(tree, node->left, data);
    } else if (cmp > 0) {
        node->right = TreeInsertNode(tree, node->right, data);
    } else {
        // Same data, update data (shouldn't happen in normal operation)
        node->data = data;
        return node;
    }

    // Update height
    TreeUpdateHeight(node);

    // Balance the tree
    int balance = TreeBalance(node);

    // Left Left Case (data < node->left->data)
    if (balance > 1 && node->left && TreeDataPrior(data, node->left->data) < 0) {
        return TreeRotateRight(node);
    }

    // Right Right Case (data > node->right->data)
    if (balance < -1 && node->right && TreeDataPrior(node->right->data, data) < 0) {
        return TreeRotateLeft(node);
    }

    // Left Right Case (data > node->left->data)
    if (balance > 1 && node->left && TreeDataPrior(node->left->data, data) < 0) {
        node->left = TreeRotateLeft(node->left);
        return TreeRotateRight(node);
    }

    // Right Left Case (data < node->right->data)
    if (balance < -1 && node->right && TreeDataPrior(data, node->right->data) < 0) {
        node->right = TreeRotateRight(node->right);
        return TreeRotateLeft(node);
    }

    return node;
}

// Insert data into the tree
TreeNodeP TreeInsert(QuickTree* tree, void* data) {
    tree->root = TreeInsertNode(tree, tree->root, data);
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
TreeNodeP TreeDeleteNode(QuickTree* tree, TreeNodeP node, void* data) {
    if (!node) return NULL;

    int cmp = TreeDataPrior(data, node->data);
    if (cmp < 0) {
        node->left = TreeDeleteNode(tree, node->left, data);
    } else if (cmp > 0) {
        node->right = TreeDeleteNode(tree, node->right, data);
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
            node->data = temp->data;
            node->right = TreeDeleteNode(tree, node->right, temp->data);
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
TreeNodeP TreeDelete(QuickTree* tree, void* data) {
    tree->root = TreeDeleteNode(tree, tree->root, data);
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

// Check if data exists in the tree
Boolean TreeContains(QuickTree* tree, void* data) {
    TreeNodeP node = tree->root;
    while (node) {
        int cmp = TreeDataPrior(data, node->data);
        if (cmp == 0) {
            return true;
        } else if (cmp < 0) {
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
