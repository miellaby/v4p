#include <stdio.h>
#include <stdlib.h>
#include "../quick/sorted.h"

int main() {
    printf("Testing TreeReset functionality...\n");
    
    // Create a tree
    QuickTree* tree = TreeNew();
    
    // Test multiple reset cycles to ensure no memory leaks or corruption
    for (int cycle = 0; cycle < 5; cycle++) {
        printf("Cycle %d:\n", cycle + 1);
        
        // Insert some data
        int data1 = 100 + cycle;
        int data2 = 200 + cycle;
        int data3 = 300 + cycle;
        
        TreeInsert(tree, &data1, 1);
        TreeInsert(tree, &data2, 2);
        TreeInsert(tree, &data3, 3);
        
        // Verify data is present
        printf("  Contains key 1: %s\n", TreeContains(tree, 1) ? "yes" : "no");
        printf("  Contains key 2: %s\n", TreeContains(tree, 2) ? "yes" : "no");
        printf("  Contains key 3: %s\n", TreeContains(tree, 3) ? "yes" : "no");
        
        // Find max
        void* maxData = TreeFindMax(tree);
        if (maxData) {
            printf("  Max data value: %d\n", *(int*)maxData);
        }
        
        // Reset the tree
        TreeReset(tree);
        
        // Verify tree is empty
        printf("  After reset - Contains key 1: %s\n", TreeContains(tree, 1) ? "yes" : "no");
        printf("  After reset - Max data: %p\n", TreeFindMax(tree));
    }
    
    // Clean up
    TreeDestroy(tree);
    printf("Test completed successfully! No crashes detected.\n");
    
    return 0;
}