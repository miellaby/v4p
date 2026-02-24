/**
 * Test for UInt32 depth support
 * This test verifies that the new balanced tree implementation
 * supports the full UInt32 depth range instead of being limited to 32 layers
 */

#include "v4p.h"
#include "g4p.h"
#include <stdio.h>

Boolean g4p_onInit(int quality, Boolean fullscreen) {
    v4p_init2(quality, fullscreen);
    v4p_setBGColor(V4P_BLACK);
    
    printf("Testing UInt32 depth range support...\n");
    
    // Test depth values that would have been truncated in the old 32-layer system
    V4pLayer testDepths[] = {31, 32, 64, 128, 256, 512, 1024, 10000, 100000, 1000000, 0xFFFFFFFF};
    int numTests = sizeof(testDepths) / sizeof(testDepths[0]);
    
    for (int i = 0; i < numTests; i++) {
        V4pLayer depth = testDepths[i];
        V4pPolygonP p = v4p_addNew(V4P_ABSOLUTE, V4P_WHITE, depth);
        v4p_rect(p, 10 + i*20, 10, 30 + i*20, 30);
        
        V4pLayer retrievedDepth = v4p_getLayer(p);
        printf("Depth %u -> %u (%s)\n", depth, retrievedDepth, 
               depth == retrievedDepth ? "PASS" : "FAIL");
        
        if (depth != retrievedDepth) {
            printf("ERROR: Depth mismatch! Expected %u, got %u\n", depth, retrievedDepth);
            return failure;
        }
    }
    
    printf("All depth tests passed! UInt32 depth support is working.\n");
    return success;
}

Boolean g4p_onTick(Int32 deltaTime) {
    // Test dynamic depth changes
    static int frameCount = 0;
    frameCount++;
    
    if (frameCount % 100 == 0) {
        // Change depth of first polygon to test dynamic updates
        V4pPolygonP p = v4p_getScene()->polygons;
        if (p) {
            V4pLayer newDepth = frameCount % 100000;  // Test with large depth values
            v4p_setLayer(p, newDepth);
            V4pLayer retrieved = v4p_getLayer(p);
            if (newDepth != retrieved) {
                printf("Dynamic depth update failed: %u != %u\n", newDepth, retrieved);
                return failure;
            }
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