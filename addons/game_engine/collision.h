#ifndef COLLISION_POINTS_H
#define COLLISION_POINTS_H

#include "g4p.h"
#include "quick/quicktable.h"

/**
 * Collision point averaging system
 * Stores average collision points per polygon pair
 */

// Collision point data for a polygon pair
typedef struct collision_point_data_s {
    V4pCoord x_sum;      // Sum of x coordinates
    V4pCoord y_sum;      // Sum of y coordinates  
    UInt16 count;        // Number of collision points
    V4pCoord avg_x;      // Average x coordinate
    V4pCoord avg_y;      // Average y coordinate
    V4pPolygonP p1;      // First polygon
    V4pPolygonP p2;      // Second polygon
} CollisionPointData;

// Callback function type for collision point finalization
typedef void (*G4pCollisionCallback)(V4pPolygonP p1, V4pPolygonP p2, V4pCoord avg_x, V4pCoord avg_y, UInt16 count);

// Initialize collision point system
typedef struct collision_points_system_s {
    QuickTable table;     // QuickTable to store collision point data
    size_t table_size;   // Size of the QuickTable
    G4pCollisionCallback callback; // Callback function for finalization
} CollisionPointsSystem;

// Function prototypes
void g4p_initCollisions(size_t table_size);
void g4p_resetCollisions();
void g4p_finalizeCollisions();
void g4p_destroyCollisions();
void g4p_setCollisionCallback(G4pCollisionCallback callback);

// Get average collision point for a polygon pair
Boolean g4p_getCollisionPoint(V4pPolygonP p1, V4pPolygonP p2, V4pCoord* avg_x, V4pCoord* avg_y);

// Add collision point data for a polygon pair
void g4p_addCollisionPoint(V4pPolygonP p1, V4pPolygonP p2, V4pCoord x, V4pCoord y);

#endif  // COLLISION_POINTS_H