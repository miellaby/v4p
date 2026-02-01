#include <stdio.h>
#include "v4p.h"
#include "collision.h"

// Simple test to demonstrate collision debugging
int main(int argc, char* argv[]) {
    printf("=== COLLISION TEST ===\n");
    
    // Initialize the system
    v4p_init2(V4P_QUALITY_NORMAL, V4P_UX_NORMAL);
    v4p_setBGColor(V4P_GREEN);
    
    // Initialize collision system
    g4p_initCollide();

    // Set default callback if none is set
    v4p_setCollideCallback(g4p_onCollide);

    // Create some polygons that should collide
    V4pPolygonP poly1 = v4p_addNew(V4P_RELATIVE, V4P_RED, 10);
    V4pPolygonP poly2 = v4p_addNew(V4P_RELATIVE, V4P_BLUE, 11);

    // they collide on layer 1
    v4p_concrete(poly1, 1);
    v4p_concrete(poly2, 2);

    // Make them overlap to trigger collisions
    v4p_rect(poly1, 100, 100, 200, 200);  // Red square
    v4p_rect(poly2, 150, 150, 250, 250);  // Blue square (overlaps with red)
    
    printf("Created overlapping polygons: red=%p, blue=%p\n", (void*)poly1, (void*)poly2);
    
    printf("\n=== RENDERING SINGLE FRAME ===\n");
    
    // Reset collisions
    g4p_resetCollide();
    
    // Render frame (this should trigger collision detection)
    v4p_render();
    
    // Finalize collisions
    g4p_finalizeCollide();
    
    // Check if any collisions were detected
    printf("\n=== COLLISION RESULTS ===\n");
    int collision_found = 0;
    for (int i = 0; i < 16; i++) {
        if (g4p_collides[i].q > 0) {
            collision_found = 1;
            printf("Collision detected on index %d: q=%d, x=%d, y=%d, poly=%p\n", 
                   i, g4p_collides[i].q, g4p_collides[i].x, g4p_collides[i].y, (void*)g4p_collides[i].poly);
            
            // Check if it's one of our test polygons
            if (g4p_collides[i].poly == poly1) {
                printf("  -> Collision with RED polygon!\n");
            } else if (g4p_collides[i].poly == poly2) {
                printf("  -> Collision with BLUE polygon!\n");
            }
        }
    }
    
    if (!collision_found) {
        printf("No collisions detected\n");
    }
    
    // Clean up
    v4p_quit();
    
    printf("\n=== TEST COMPLETE ===\n");
    return 0;
}