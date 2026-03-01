#include <stdio.h>
#include <stdlib.h>
#include "../quick/sorted.h"

// Comparison function for int data
static int intComparator(void* a, void* b) {
    int valA = *(int*)a;
    int valB = *(int*)b;
    if (valA < valB) return -1;
    if (valA > valB) return 1;
    return 0;
}

// Forward declaration
void testTreeReset();

int main() {
    // Test basic tree functionality
    {
    QuickTree* tree = TreeNew();
    if (!tree) {
        printf("Failed to create tree\n");
        return 1;
    }

    // Set comparator for int data
    TreeSetCompareFunc(intComparator);
    
    // Insert some data
    int data1 = 1, data2 = 2, data3 = 3;
    TreeInsert(tree, &data1);
    TreeInsert(tree, &data2);
    TreeInsert(tree, &data3);

    // Check if data exists
    printf("Tree contains data 1: %s\n", TreeContains(tree, &data1) ? "yes" : "no");
    printf("Tree contains data 2: %s\n", TreeContains(tree, &data2) ? "yes" : "no");
    printf("Tree contains data 3: %s\n", TreeContains(tree, &data3) ? "yes" : "no");
    
    int data4 = 4;
    printf("Tree contains data 4: %s\n", TreeContains(tree, &data4) ? "yes" : "no");

    // Find max
    void* maxData = TreeFindMax(tree);
    if (maxData) {
        printf("Max data: %d\n", *(int*)maxData);
    }

    // Delete a node
    TreeDelete(tree, &data2);
    printf("After deleting data 2, contains data 2: %s\n", TreeContains(tree, &data2) ? "yes" : "no");

    // Clean up
    TreeDestroy(tree);
    printf("Test completed successfully!\n");
    }
    
    // Test TreeReset functionality
    testTreeReset();
    
    return 0;
}

// Test TreeReset functionality
void testTreeReset() {
    printf("\n=== Testing TreeReset ===\n");
    
    // Create a tree and add some data
    QuickTree* tree = TreeNew();
    
    // Set comparator for int data
    TreeSetCompareFunc(intComparator);
    
    // Insert some test data
    int data1 = 1, data2 = 2, data3 = 3;
    TreeInsert(tree, &data1);
    TreeInsert(tree, &data2);
    TreeInsert(tree, &data3);
    
    // Verify data is in the tree
    printf("Before reset - Tree contains data 1: %s\n", TreeContains(tree, &data1) ? "yes" : "no");
    printf("Before reset - Tree contains data 2: %s\n", TreeContains(tree, &data2) ? "yes" : "no");
    printf("Before reset - Tree contains data 3: %s\n", TreeContains(tree, &data3) ? "yes" : "no");
    printf("Before reset - Tree max data: %p\n", TreeFindMax(tree));
    
    // Reset the tree
    TreeReset(tree);
    
    // Verify tree is empty
    printf("After reset - Tree contains data 1: %s\n", TreeContains(tree, &data1) ? "yes" : "no");
    printf("After reset - Tree contains data 2: %s\n", TreeContains(tree, &data2) ? "yes" : "no");
    printf("After reset - Tree contains data 3: %s\n", TreeContains(tree, &data3) ? "yes" : "no");
    printf("After reset - Tree max data: %p\n", TreeFindMax(tree));
    
    // Insert new data after reset
    int data4 = 4;
    TreeInsert(tree, &data4);
    
    // Verify new data is in the tree
    printf("After reset and new insert - Tree contains data 4: %s\n", TreeContains(tree, &data4) ? "yes" : "no");
    printf("After reset and new insert - Tree max data: %p\n", TreeFindMax(tree));
    
    // Clean up
    TreeDestroy(tree);
    
    printf("TreeReset test completed successfully!\n");
}