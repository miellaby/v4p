#ifndef SORTED_H
#define SORTED_H
/**
 * Sorted AVL Tree
 * Generic AVL tree implementation for sorted data
 */
#include "v4p_ll.h"
#include "quick/heap.h"

// A settable function to compare tree data. Please set it before using!
// Must return <0 if a<b, 0 if a==b, >0 if a>b
extern int (*TreeCompareFunc)(void*, void*);

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
TreeNodeP TreeInsert(QuickTree* tree, void* data);

// Delete data from the tree
TreeNodeP TreeDelete(QuickTree* tree, void* data);

// Find the maximum data in the tree
void* TreeFindMax(QuickTree* tree);

// Find the minimum data in the tree
void* TreeFindMin(QuickTree* tree);

// Check if data exists in the tree
Boolean TreeContains(QuickTree* tree, void* data);

// Reset the tree by clearing all nodes and resetting the root
void TreeReset(QuickTree* tree);

// macro to set the data comparator, defaults to TreeDefaultComparator if NULL
#define TreeSetCompareFunc(p) TreeCompareFunc = (p)

#endif // SORTED_H
