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

int main() {
    printf("Testing TreeFindMin functionality...\n");
    
    // Create a tree
    QuickTree* tree = TreeNew();
    
    // Set comparator for int data
    TreeSetCompareFunc(intComparator);
    
    // Insert some data
    int data1 = 25, data2 = 50, data3 = 100, data4 = 150, data5 = 200;
    TreeInsert(tree, &data1);
    TreeInsert(tree, &data2);
    TreeInsert(tree, &data3);
    TreeInsert(tree, &data4);
    TreeInsert(tree, &data5);
    
    // Test TreeFindMin
    void* minData = TreeFindMin(tree);
    if (minData) {
        printf("Min data value: %d (expected: 25)\n", *(int*)minData);
        if (*(int*)minData == 25) {
            printf("✓ TreeFindMin works correctly!\n");
        } else {
            printf("✗ TreeFindMin returned wrong value!\n");
            return 1;
        }
    } else {
        printf("✗ TreeFindMin returned NULL!\n");
        return 1;
    }
    
    // Test TreeFindMax for comparison
    void* maxData = TreeFindMax(tree);
    if (maxData) {
        printf("Max data value: %d (expected: 200)\n", *(int*)maxData);
        if (*(int*)maxData == 200) {
            printf("✓ TreeFindMax works correctly!\n");
        } else {
            printf("✗ TreeFindMax returned wrong value!\n");
            return 1;
        }
    } else {
        printf("✗ TreeFindMax returned NULL!\n");
        return 1;
    }
    
    // Test TreeFindMin on empty tree
    TreeReset(tree);
    void* emptyMin = TreeFindMin(tree);
    if (emptyMin == NULL) {
        printf("✓ TreeFindMin correctly returns NULL for empty tree!\n");
    } else {
        printf("✗ TreeFindMin should return NULL for empty tree!\n");
        return 1;
    }
    
    // Clean up
    TreeDestroy(tree);
    printf("All tests passed successfully!\n");
    
    return 0;
}