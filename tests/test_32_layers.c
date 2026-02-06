#include "v4p.h"
#include "g4p.h"
#include <stdio.h>

// Test collision callback
void test_collision_callback(V4pCollisionLayer i1, V4pCollisionLayer i2, V4pCoord py, V4pCoord x1, V4pCoord x2, V4pPolygonP p1, V4pPolygonP p2) {
    printf("Collision detected between layers %u and %u\n", i1, i2);
}

Boolean g4p_onInit() {
    v4p_init();
    v4p_setBGColor(V4P_BLACK);
    
    // Test creating polygons on layers beyond 16
    printf("Testing 32-layer support...\n");
    
    // Create polygons on various layers including those > 16
    for (V4pLayer z = 0; z < 32; z++) {
        V4pPolygonP p = v4p_addNew(V4P_ABSOLUTE, V4P_WHITE, z);
        v4p_rect(p, z * 10, z * 10, z * 10 + 50, z * 10 + 50);
        
        // Set collision mask for odd layers
        if (z % 2 == 1) {
            v4p_setCollisionMask(p, 1 << (z % 32));
        }
        
        printf("Created polygon on layer %u\n", z);
    }
    
    // Set collision callback
    v4p_setCollisionCallback(test_collision_callback);
    
    return success;
}

Boolean g4p_onTick(Int32 deltaTime) {
    // No special logic needed for this test
    return success;
}

Boolean g4p_onFrame() {
    return v4p_render();
}

void g4p_onQuit() {
    v4p_quit();
}

int main(int argc, char** argv) {
    printf("Starting 32-layer test...\n");
    return g4p_main(argc, argv);
}