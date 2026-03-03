#include "v4p.h"
#include "_v4p.h"
#include <stdio.h>

int main() {
    v4p_init();
    
    // Create a polygon (e.g., a square)
    V4pPolygonP poly = v4p_addNew(V4P_ABSOLUTE, V4P_WHITE, 4);
    v4p_addPoint(poly, 100, 100);
    v4p_addPoint(poly, 200, 100);
    v4p_addPoint(poly, 200, 200);
    v4p_addPoint(poly, 100, 200);
    
    // Print original polygon points
    printf("Original polygon points:\n");
    Polygon* originalPoly = (Polygon*)poly;
    V4pPointP point = originalPoly->point1;
    int count = 0;
    while (point) {
        printf("Point %d: (%d, %d)\n", count++, point->x, point->y);
        point = point->next;
    }
    
    // Clip the polygon (adjust bounds to intersect with the polygon)
    V4pPolygonP clipped = v4p_clip(poly, 150, 150, 250, 250);
    
    if (clipped) {
        printf("Clipping successful.\n");
        Polygon* clippedPoly = (Polygon*)clipped;
        V4pPointP point = clippedPoly->point1;
        int count = 0;
        printf("Clipped polygon points:\n");
        while (point) {
            printf("Point %d: (%d, %d)\n", count++, point->x, point->y);
            point = point->next;
        }
        if (count == 0) {
            printf("No points in clipped polygon.\n");
        }
    } else {
        printf("Clipping failed.\n");
    }
    
    v4p_quit();
    return 0;
}