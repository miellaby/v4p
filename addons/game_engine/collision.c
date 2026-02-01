#include "collision.h"
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
    g4p_collides[i1].q += l;
    g4p_collides[i1].x += dx;
    g4p_collides[i1].y += dy;
    g4p_collides[i1].poly = p2;
    g4p_collides[i2].q += l;
    g4p_collides[i2].x += dx;
    g4p_collides[i2].y += dy;
    g4p_collides[i2].poly = p1;
    v4pi_debug("After collision: collides[%d].q=%d, collides[%d].q=%d\n",
               i1,
               g4p_collides[i1].q,
               i2,
               g4p_collides[i2].q);
}

// Initialize collision system (sets up callback)
void g4p_initCollide() {
    // Set default callback if none is set
    v4p_setCollideCallback(g4p_onCollide);

    // Also perform initial reset
    g4p_resetCollide();
}

// Reset collision data (called before each frame)
void g4p_resetCollide() {
    int i;
    v4pi_debug("g4p_resetCollide() called\n");
    for (i = 0; i < 16; i++) {
        g4p_collides[i].q = 0;
        g4p_collides[i].x = 0;
        g4p_collides[i].y = 0;
        g4p_collides[i].poly = NULL;
    }
}

// Finalize collision system (process collision data)
void g4p_finalizeCollide() {
    int i;
    v4pi_debug("g4p_finalizeCollide() called\n");
    for (i = 0; i < 16; i++) {
        v4pi_debug("collides[%d]: q=%d, x=%d, y=%d, poly=%p\n",
                   i,
                   g4p_collides[i].q,
                   g4p_collides[i].x,
                   g4p_collides[i].y,
                   (void*) g4p_collides[i].poly);
        if (! g4p_collides[i].q)
            continue;
        g4p_collides[i].x /= g4p_collides[i].q;
        g4p_collides[i].y /= g4p_collides[i].q;
        v4pi_debug("collides[%d] after division: x=%d, y=%d\n",
                   i,
                   g4p_collides[i].x,
                   g4p_collides[i].y);
    }
}
