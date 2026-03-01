#ifndef SORTED_H
#define SORTED_H
/**
 * Sorted AVL Tree
 * Generic AVL tree implementation for sorted data
 */
#include "v4p_ll.h"
#include "quick/heap.h"

// A settable function to compare tree nodes. Please set it before using!
// Must return (arg1 < arg2)
extern int (*TreeDataPrior)(UInt32, UInt32);

typedef struct sTreeNode* TreeNodeP;

// AVL Tree
typedef struct sTree {
    TreeNodeP root;
    QuickHeap nodeHeap;
} QuickTree;

// Create a new tree
QuickTree* TreeNew();

// Free a tree
void TreeDestroy(QuickTree* tree);

// Insert data into the tree
TreeNodeP TreeInsert(QuickTree* tree, void* data, UInt32 key);

// Delete data from the tree
TreeNodeP TreeDelete(QuickTree* tree, UInt32 key);

// Find the maximum key in the tree
void* TreeFindMax(QuickTree* tree);

// Find the minimum key in the tree
void* TreeFindMin(QuickTree* tree);

// Check if a key exists in the tree
Boolean TreeContains(QuickTree* tree, UInt32 key);

// Reset the tree by clearing all nodes and resetting the root
void TreeReset(QuickTree* tree);

// Default key comparator (key=value ordering)
int TreeDefaultKeyComparator(UInt32 a, UInt32 b);

// macro to set the key comparator, defaults to TreeDefaultKeyComparator if NULL
#define TreeSetDataPrior(p) TreeDataPrior = (p ? p : TreeDefaultKeyComparator)

#endif // SORTED_H
