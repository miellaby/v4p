#ifndef COLLISION_H
#define COLLISION_H

#include "v4p_ll.h"
#include "v4p.h"

/**
 * Collision detection structures and functions
 */

typedef struct collide_s {
    V4pCoord x;
    V4pCoord y;
    UInt16 q;
    V4pPolygonP poly;
} Collide;

// Collision callback function type
typedef Boolean (*V4pCollideCallback)(V4pCollide i1,
                                      V4pCollide i2,
                                      V4pCoord py,
                                      V4pCoord x1,
                                      V4pCoord x2,
                                      V4pPolygonP p1,
                                      V4pPolygonP p2);

// Global collision array (moved from backends to game engine)
extern Collide collides[16];

// Collision callback functions
void v4p_setCollideCallback(V4pCollideCallback callback);
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