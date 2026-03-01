#include <stdio.h>
#include <stdlib.h>
#include "../quick/sorted.h"

int main() {
    printf("Testing TreeFindMin functionality...\n");
    
    // Create a tree
    QuickTree* tree = TreeNew();
    
    // Insert some data with different keys
    int data1 = 100, data2 = 200, data3 = 50, data4 = 150, data5 = 25;
    TreeInsert(tree, &data1, 100);  // key=100, data=100
    TreeInsert(tree, &data2, 200);  // key=200, data=200
    TreeInsert(tree, &data3, 50);   // key=50, data=50
    TreeInsert(tree, &data4, 150);  // key=150, data=150
    TreeInsert(tree, &data5, 25);   // key=25, data=25
    
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