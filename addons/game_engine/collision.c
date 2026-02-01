#include "collision.h"
#include "collision_points.h"
#include "g4p.h"
#include "v4pi.h"

// Global collision array
Collide g4p_collides[16];

// Default collision callback
void g4p_onCollide(V4pCollide i1,
                   V4pCollide i2,
                   V4pCoord py,
                   V4pCoord x1,
                   V4pCoord x2,
                   V4pPolygonP p1,
                   V4pPolygonP p2) {
    int l, dx, dy;
    l = x2 - x1;
    dx = x1 * l + (l + 1) * l / 2;
    dy = l * py;
    v4pi_debug("g4p_onCollide() called with i1=%d, i2=%d, l=%d, p1=%p, p2=%p\n",
               i1,
               i2,
               l,
               (void*) p1,
               (void*) p2);
    
    // Original collision accumulation (per layer)
    g4p_collides[i1].q += l;
    g4p_collides[i1].x += dx;
    g4p_collides[i1].y += dy;
    g4p_collides[i1].poly = p2;
    g4p_collides[i2].q += l;
    g4p_collides[i2].x += dx;
    g4p_collides[i2].y += dy;
    g4p_collides[i2].poly = p1;
    
    // Compute collision point (midpoint of the collision segment)
    V4pCoord collision_x = (x1 + x2) / 2;
    V4pCoord collision_y = py;
    
    // Store collision point for this polygon pair
    g4p_addCollisionPoint(p1, p2, collision_x, collision_y);
    
    v4pi_debug("After collision: collides[%d].q=%d, collides[%d].q=%d\n",
               i1,
               g4p_collides[i1].q,
               i2,
               g4p_collides[i2].q);
    v4pi_debug("Stored collision point (%d, %d) for polygon pair (%p, %p)\n",
               collision_x, collision_y, (void*)p1, (void*)p2);
}

// Initialize old collision system
void g4p_initCollide() {
    // Also perform initial reset
    g4p_resetCollide();
}

// Reset collision data (called before each frame)
void g4p_resetCollide() {
    int collisionLayer;
    v4pi_debug("g4p_resetCollide() called\n");
    for (collisionLayer = 0; collisionLayer < 16; collisionLayer++) {
        g4p_collides[collisionLayer].q = 0;
        g4p_collides[collisionLayer].x = 0;
        g4p_collides[collisionLayer].y = 0;
        g4p_collides[collisionLayer].poly = NULL;
    }
    
    // Reset collision points system
    g4p_resetCollisionPoints();
}

// Finalize collision system (process collision data)
void g4p_finalizeCollide() {
    int collisionLayer;
    v4pi_debug("g4p_finalizeCollide() called\n");
    for (collisionLayer = 0; collisionLayer < 16; collisionLayer++) {
        v4pi_debug("collides[%d]: q=%d, x=%d, y=%d, poly=%p\n",
                   collisionLayer,
                   g4p_collides[collisionLayer].q,
                   g4p_collides[collisionLayer].x,
                   g4p_collides[collisionLayer].y,
                   (void*) g4p_collides[collisionLayer].poly);
        if (! g4p_collides[collisionLayer].q)
            continue;
        g4p_collides[collisionLayer].x /= g4p_collides[collisionLayer].q;
        g4p_collides[collisionLayer].y /= g4p_collides[collisionLayer].q;
        v4pi_debug("collides[%d] after division: x=%d, y=%d\n",
                   collisionLayer,
                   g4p_collides[collisionLayer].x,
                   g4p_collides[collisionLayer].y);
    }
    
    // Finalize collision points system (compute averages)
    g4p_finalizeCollisionPoints();
}
