#include <stdio.h>
#include "v4p.h"
#include "collision.h"
#include "g4p.h"

// Simple test to demonstrate collision debugging

// Stub callbacks required by the game engine
Boolean g4p_onInit() {
    printf("=== COLLISION TEST INIT ===\n");
    
    // Initialize the system
    v4p_init2(V4P_QUALITY_NORMAL, V4P_UX_NORMAL);
    v4p_setBGColor(V4P_GREEN);
    
    // Initialize collision system
    g4p_resetCollisions();

    // Set default callback if none is set
    v4p_setCollisionCallback(g4p_onCollide);

    // Create some polygons that should collide
    V4pPolygonP poly1 = v4p_addNew(V4P_RELATIVE, V4P_RED, 10);
    V4pPolygonP poly2 = v4p_addNew(V4P_RELATIVE, V4P_BLUE, 11);

    // they collide on layer 1
    v4p_setCollisionMask(poly1, 1);
    v4p_setCollisionMask(poly2, 2);

    // Make them overlap to trigger collisions
    v4p_rect(poly1, 100, 100, 200, 200);  // Red square
    v4p_rect(poly2, 150, 150, 250, 250);  // Blue square (overlaps with red)
    
    printf("Created overlapping polygons: red=%p, blue=%p\n", (void*)poly1, (void*)poly2);
    
    return success;
}

Boolean g4p_onTick(Int32 deltaTime) {
    // No physics needed for this test
    return success;
}

Boolean g4p_onFrame() {
    static int frame_count = 0;
    frame_count++;
    
    // Reset collisions
    g4p_resetCollisions();

    // Render frame (this should trigger collision detection)
    v4p_render();
    
    // Finalize collisions
    g4p_finalizeCollisions();
    
    // Print results after first frame
    if (frame_count == 1) {
        printf("\n=== TEST COMPLETE ===\n");
    }
    
    return success;
}

void g4p_onQuit() {
    // Clean up
    v4p_quit();
}

int main(int argc, char* argv[]) {
    printf("=== COLLISION TEST ===\n");
    printf("Running collision detection test...\n");
    return g4p_main(argc, argv);
}