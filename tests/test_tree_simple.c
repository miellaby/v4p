#include <stdio.h>
#include <stdlib.h>
#include "sorted.h"

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

    // Insert some data
    int data1 = 42, data2 = 84, data3 = 16;
    TreeInsert(tree, &data1, 1);
    TreeInsert(tree, &data2, 2);
    TreeInsert(tree, &data3, 3);

    // Check if keys exist
    printf("Tree contains key 1: %s\n", TreeContains(tree, 1) ? "yes" : "no");
    printf("Tree contains key 2: %s\n", TreeContains(tree, 2) ? "yes" : "no");
    printf("Tree contains key 3: %s\n", TreeContains(tree, 3) ? "yes" : "no");
    printf("Tree contains key 4: %s\n", TreeContains(tree, 4) ? "yes" : "no");

    // Find max
    void* maxData = TreeFindMax(tree);
    if (maxData) {
        printf("Max data: %d\n", *(int*)maxData);
    }

    // Delete a node
    TreeDelete(tree, 2);
    printf("After deleting key 2, contains key 2: %s\n", TreeContains(tree, 2) ? "yes" : "no");

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
    
    // Insert some test data
    int data1 = 42, data2 = 100, data3 = 7;
    TreeInsert(tree, &data1, 1);
    TreeInsert(tree, &data2, 2);
    TreeInsert(tree, &data3, 3);
    
    // Verify data is in the tree
    printf("Before reset - Tree contains key 1: %s\n", TreeContains(tree, 1) ? "yes" : "no");
    printf("Before reset - Tree contains key 2: %s\n", TreeContains(tree, 2) ? "yes" : "no");
    printf("Before reset - Tree contains key 3: %s\n", TreeContains(tree, 3) ? "yes" : "no");
    printf("Before reset - Tree max data: %p\n", TreeFindMax(tree));
    
    // Reset the tree
    TreeReset(tree);
    
    // Verify tree is empty
    printf("After reset - Tree contains key 1: %s\n", TreeContains(tree, 1) ? "yes" : "no");
    printf("After reset - Tree contains key 2: %s\n", TreeContains(tree, 2) ? "yes" : "no");
    printf("After reset - Tree contains key 3: %s\n", TreeContains(tree, 3) ? "yes" : "no");
    printf("After reset - Tree max data: %p\n", TreeFindMax(tree));
    
    // Insert new data after reset
    int data4 = 200;
    TreeInsert(tree, &data4, 4);
    
    // Verify new data is in the tree
    printf("After reset and new insert - Tree contains key 4: %s\n", TreeContains(tree, 4) ? "yes" : "no");
    printf("After reset and new insert - Tree max data: %p\n", TreeFindMax(tree));
    
    // Clean up
    TreeDestroy(tree);
    
    printf("TreeReset test completed successfully!\n");
}