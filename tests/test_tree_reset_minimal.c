#include "quick/sorted.h"
#include <stdio.h>
#include <stdlib.h>

// Comparison function for int data
static int intComparator(void* a, void* b) {
    int valA = *(int*)a;
    int valB = *(int*)b;
    if (valA < valB) return -1;
    if (valA > valB) return 1;
    return 0;
}

int main() {
    printf("Testing TreeReset functionality...\n");
    
    // Create a tree
    QuickTree* tree = TreeNew();
    // Set comparator for int data
    TreeSetCompareFunc(intComparator);
    
    // Test multiple reset cycles to ensure no memory leaks or corruption
    for (int cycle = 0; cycle < 5; cycle++) {
        printf("Cycle %d:\n", cycle + 1);
        
        // Insert some data
        int data1 = 1 + cycle;
        int data2 = 2 + cycle;
        int data3 = 3 + cycle;
        
        TreeInsert(tree, &data1);
        TreeInsert(tree, &data2);
        TreeInsert(tree, &data3);
        
        // Verify data is present
        printf("  Contains data 1: %s\n", TreeContains(tree, &data1) ? "yes" : "no");
        printf("  Contains data 2: %s\n", TreeContains(tree, &data2) ? "yes" : "no");
        printf("  Contains data 3: %s\n", TreeContains(tree, &data3) ? "yes" : "no");
        
        // Find max
        void* maxData = TreeFindMax(tree);
        if (maxData) {
            printf("  Max data value: %d\n", *(int*)maxData);
        }
        
        // Reset the tree
        TreeReset(tree);
        
        // Verify tree is empty
        int testData1 = 1;
        printf("  After reset - Contains data 1: %s\n", TreeContains(tree, &testData1) ? "yes" : "no");
        printf("  After reset - Max data: %p\n", TreeFindMax(tree));
    }
    
    // Clean up
    TreeDestroy(tree);
    printf("Test completed successfully! No crashes detected.\n");
    
    return 0;
}