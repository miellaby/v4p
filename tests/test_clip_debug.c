#include "v4p.h"
#include "_v4p.h"
#include <stdio.h>
#include "clipping/clipping.h"

int main() {
    v4p_init();
    
    // Create a simple square polygon
    V4pPolygonP poly = v4p_addNew(V4P_ABSOLUTE, V4P_WHITE, 4);
    v4p_addPoint(poly, 100, 100);  // This becomes the last point due to reverse order
    v4p_addPoint(poly, 200, 100);
    v4p_addPoint(poly, 200, 200);
    v4p_addPoint(poly, 100, 200);  // This becomes the first point
    
    // Print original polygon points in order
    printf("Original polygon points (stored in reverse order):\n");
    Polygon* originalPoly = (Polygon*)poly;
    V4pPointP point = originalPoly->point1;
    int count = 0;
    while (point) {
        printf("Point %d: (%d, %d)\n", count++, point->x, point->y);
        point = point->next;
    }
    
    // Test clipping with bounds that should intersect the square
    printf("\nClipping with bounds (150,150,250,250):\n");
    V4pPolygonP clipped = v4p_clip(poly, 150, 150, 250, 250);
    
    if (clipped) {
        printf("Clipping successful.\n");
        Polygon* clippedPoly = (Polygon*)clipped;
        V4pPointP clippedPoint = clippedPoly->point1;
        int clippedCount = 0;
        printf("Clipped polygon points:\n");
        while (clippedPoint) {
            printf("Point %d: (%d, %d)\n", clippedCount++, clippedPoint->x, clippedPoint->y);
            clippedPoint = clippedPoint->next;
        }
        if (clippedCount == 0) {
            printf("No points in clipped polygon.\n");
        }
        
        // Expected result: should have 4 points forming a smaller square
        // (150,150), (200,150), (200,200), (150,200)
        printf("\nExpected: 4 points forming intersection rectangle\n");
    } else {
        printf("Clipping failed.\n");
    }
    
    v4p_quit();
    return 0;
}