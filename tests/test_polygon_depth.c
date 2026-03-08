#include "quick/sorted.h"
#include "v4p.h"
#include "_v4p.h"
#include <stdio.h>
#include <stdlib.h>

// Comparison function for polygons based on depth (z layer)
static int polygonDepthComparator(void* a, void* b) {
    V4pPolygonP polyA = (V4pPolygonP)a;
    V4pPolygonP polyB = (V4pPolygonP)b;
    
    if (polyA->z < polyB->z) return -1;
    if (polyA->z > polyB->z) return 1;
    return 0;
}

int main() {
    printf("Testing polygon depth comparison functionality...\n");
    
    // Create a tree
    QuickTree* tree = TreeNew();
    TreeSetCompareFunc(polygonDepthComparator);
    
    // Create some mock polygons with different depths
    // Note: We can't use v4p_new directly here without full initialization,
    // so we'll create minimal polygon structures for testing
    Polygon poly1, poly2, poly3;
    
    poly1.z = 10;
    poly2.z = 5;
    poly3.z = 15;
    
    // Insert polygons
    TreeInsert(tree, &poly1);
    TreeInsert(tree, &poly2);
    TreeInsert(tree, &poly3);
    
    // Test contains
    printf("Tree contains poly1 (z=10): %s\n", TreeContains(tree, &poly1) ? "yes" : "no");
    printf("Tree contains poly2 (z=5): %s\n", TreeContains(tree, &poly2) ? "yes" : "no");
    printf("Tree contains poly3 (z=15): %s\n", TreeContains(tree, &poly3) ? "yes" : "no");
    
    // Test find max (should be highest z)
    Polygon* maxPoly = (Polygon*)TreeFindMax(tree);
    if (maxPoly) {
        printf("Max polygon z value: %d (expected: 15)\n", maxPoly->z);
        if (maxPoly->z == 15) {
            printf("✓ TreeFindMax works correctly for polygons!\n");
        } else {
            printf("✗ TreeFindMax returned wrong polygon!\n");
            TreeDestroy(tree);
            return 1;
        }
    } else {
        printf("✗ TreeFindMax returned NULL!\n");
        TreeDestroy(tree);
        return 1;
    }
    
    // Test find min (should be lowest z)
    Polygon* minPoly = (Polygon*)TreeFindMin(tree);
    if (minPoly) {
        printf("Min polygon z value: %d (expected: 5)\n", minPoly->z);
        if (minPoly->z == 5) {
            printf("✓ TreeFindMin works correctly for polygons!\n");
        } else {
            printf("✗ TreeFindMin returned wrong polygon!\n");
            TreeDestroy(tree);
            return 1;
        }
    } else {
        printf("✗ TreeFindMin returned NULL!\n");
        TreeDestroy(tree);
        return 1;
    }
    
    // Test delete
    TreeDelete(tree, &poly2);
    printf("After deleting poly2 (z=5), contains poly2: %s\n", TreeContains(tree, &poly2) ? "yes" : "no");
    
    // Clean up
    TreeDestroy(tree);
    
    printf("Polygon depth comparison test completed successfully!\n");
    return 0;
}