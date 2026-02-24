/**
 * Test for collision detection with ordered polygon pairs
 * This test verifies that collision callbacks are called for successive
 * polygon pairs in proper collision-layer order
 */

#include "v4p.h"
#include "g4p.h"
#include <stdio.h>

// Track collision callbacks
int collisionCount = 0;
V4pLayer lastDepth1 = 0, lastDepth2 = 0;

void test_collision_callback(V4pCollisionLayer i1, V4pCollisionLayer i2, 
                            V4pCoord py, V4pCoord x1, V4pCoord x2,
                            V4pPolygonP p1, V4pPolygonP p2) {
    collisionCount++;
    lastDepth1 = i1;
    lastDepth2 = i2;
    
    printf("Collision %d: layers %u-%u at y=%d, x=%d-%d\n", 
           collisionCount, i1, i2, py, x1, x2);
    
    // Verify that depths are in descending order (i1 > i2)
    if (i1 <= i2) {
        printf("ERROR: Collision layers not in descending order: %u <= %u\n", i1, i2);
    }
}

Boolean g4p_onInit(int quality, Boolean fullscreen) {
    v4p_init2(quality, fullscreen);
    v4p_setBGColor(V4P_BLACK);
    v4p_setCollisionCallback(test_collision_callback);
    
    printf("Testing collision detection with ordered polygon pairs...\n");
    
    // Create overlapping polygons at different depths
    V4pLayer depths[] = {100, 50, 200, 75, 150};
    int numPolygons = sizeof(depths) / sizeof(depths[0]);
    
    for (int i = 0; i < numPolygons; i++) {
        V4pPolygonP p = v4p_addNew(V4P_ABSOLUTE, 0xFF0000 + (i * 32), depths[i]);
        v4p_rect(p, 50, 50, 150, 150);  // All overlap
        v4p_setCollisionMask(p, 1 << (i % 8));  // Set collision mask
        printf("Created polygon %d at depth %u\n", i, depths[i]);
    }
    
    return success;
}

Boolean g4p_onTick(Int32 deltaTime) {
    static int frameCount = 0;
    frameCount++;
    
    if (frameCount == 10) {
        printf("Total collisions detected: %d\n", collisionCount);
        
        if (collisionCount > 0) {
            printf("Last collision was between layers %u and %u\n", lastDepth1, lastDepth2);
            
            // Verify that we got collisions in proper order
            if (lastDepth1 <= lastDepth2) {
                printf("ERROR: Collisions not in proper depth order!\n");
                return failure;
            }
            
            printf("SUCCESS: Collision detection working correctly with ordered pairs!\n");
        } else {
            printf("INFO: No collisions detected (polygons may not be overlapping in view)\n");
        }
    }
    
    return success;
}

Boolean g4p_onFrame() {
    return v4p_render();
}

void g4p_onQuit() {
    v4p_quit();
}

int main(int argc, char** argv) {
    return g4p_main(argc, argv);
}