#ifndef COLLISION_H
#define COLLISION_H

#include "g4p.h"

/**
 * Collision detection structures and functions
 */

 // Collision system access
typedef struct collide_s {
    V4pCoord x;
    V4pCoord y;
    UInt16 q;
    V4pPolygonP poly;
} Collide;
extern Collide g4p_collides[16];

/**
 * Gather Rendering Collision detection
 */

// G4P Collision callback function
Boolean g4p_onCollide(V4pCollide i1,
                      V4pCollide i2,
                      V4pCoord py,
                      V4pCoord x1,
                      V4pCoord x2,
                      V4pPolygonP p1,
                      V4pPolygonP p2);

// Collision initialization, reset, and finalization
void g4p_initCollide();
void g4p_resetCollide();
void g4p_finalizeCollide();

#endif // COLLISION_H