#include <stdio.h>
#include "v4p.h"
#include "addons/game_engine/g4p.h"
#include "addons/game_engine/collision_points.h"

// Test state
static int collision_callback_count = 0;
static int collision_point_callback_count = 0;

// Forward declarations
void test_collision_point_callback(V4pPolygonP p1, V4pPolygonP p2, V4pCoord avg_x, V4pCoord avg_y, UInt16 count);

// Game state
V4pPolygonP poly1 = NULL;
V4pPolygonP poly2 = NULL;
V4pPolygonP poly3 = NULL;

// Game initialization
Boolean g4p_onInit() {
    printf("=== COLLISION POINTS TEST ===\n");
    
    // Set collision point callback
    g4p_setCollisionPointCallback(test_collision_point_callback);
    
    // Create some polygons that should collide
    poly1 = v4p_addNew(V4P_RELATIVE, V4P_RED, 10);
    poly2 = v4p_addNew(V4P_RELATIVE, V4P_BLUE, 11);
    poly3 = v4p_addNew(V4P_RELATIVE, V4P_YELLOW, 12);
    
    // they collide on different layers
    v4p_setCollisionMask(poly1, 1);
    v4p_setCollisionMask(poly2, 2);
    v4p_setCollisionMask(poly3, 4);
    
    // Make them overlap to trigger collisions
    v4p_rect(poly1, 100, 100, 200, 200);  // Red square
    v4p_rect(poly2, 150, 150, 250, 250);  // Blue square (overlaps with red)
    v4p_rect(poly3, 125, 125, 225, 225);  // Yellow square (overlaps with both)
    
    printf("Created overlapping polygons: red=%p, blue=%p, yellow=%p\n", 
           (void*)poly1, (void*)poly2, (void*)poly3);
    
    printf("Expected collisions:\n");
    printf("  - Red (layer 1) vs Blue (layer 2)\n");
    printf("  - Red (layer 1) vs Yellow (layer 3)\n");
    printf("  - Blue (layer 2) vs Yellow (layer 3)\n");
    
    return success;
}

// Game tick (physics/update logic)
Boolean g4p_onTick(Int32 deltaTime) {
    // No physics needed for this test
    return success;
}

// Game frame rendering
Boolean g4p_onFrame() {
    // Render the frame (this triggers collision detection)
    v4p_render();
    
    // Print test results after first frame
    static int frame_count = 0;
    frame_count++;
    
    if (frame_count == 1) {
        printf("\n=== TEST RESULTS ===\n");
        printf("Collision callback calls: %d\n", collision_callback_count);
        printf("Collision point callback calls: %d\n", collision_point_callback_count);
        
        if (collision_point_callback_count > 0) {
            printf("✓ SUCCESS: Collision point callback is working!\n");
        } else {
            printf("✗ FAILURE: Collision point callback was not called!\n");
        }
        
        // Test v4p_getCollisionMask function
        printf("\nTesting v4p_getCollisionMask():\n");
        printf("  poly1 (red) layer: %d (expected: 1)\n", v4p_getCollisionMask(poly1));
        printf("  poly2 (blue) layer: %d (expected: 2)\n", v4p_getCollisionMask(poly2));
        printf("  poly3 (yellow) layer: %d (expected: 4)\n", v4p_getCollisionMask(poly3));
    }
    
    return success;
}

// Game cleanup
void g4p_onQuit() {
    printf("Test completed.\n");
}

// Custom collision callback that also tests collision points
void custom_collision_callback(V4pCollisionLayer i1, V4pCollisionLayer i2, V4pCoord py, 
                               V4pCoord x1, V4pCoord x2, V4pPolygonP p1, V4pPolygonP p2) {
    collision_callback_count++;
    
    printf("Collision detected: layers %d vs %d, polygons %p vs %p\n", 
           i1, i2, (void*)p1, (void*)p2);
    
    // Call the standard collision callback first
    g4p_onCollide(i1, i2, py, x1, x2, p1, p2);
    
    // This callback is called for each collision segment
    // The collision points system should accumulate these
}

// Collision point callback for testing
void test_collision_point_callback(V4pPolygonP p1, V4pPolygonP p2, V4pCoord avg_x, V4pCoord avg_y, UInt16 count) {
    collision_point_callback_count++;
    
    printf("Collision point callback: polygons %p vs %p, avg=(%d,%d), count=%d\n",
           (void*)p1, (void*)p2, avg_x, avg_y, count);
    
    // Verify we can get collision layers
    V4pCollisionMask layer1 = v4p_getCollisionMask(p1);
    V4pCollisionMask layer2 = v4p_getCollisionMask(p2);
    
    printf("  Collision layers: %d vs %d\n", layer1, layer2);
}

int main(int argc, char* argv[]) {
    // Set our custom collision callback
    v4p_setCollisionCallback(custom_collision_callback);
    
    // Run the game engine
    return g4p_main(argc, argv);
}