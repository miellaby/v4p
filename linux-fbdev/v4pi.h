#ifndef V4PI
#define V4PI
/**
 * V4P Implementation IF for Linux Framebuffer
 */

#include "v4p_ll.h"
#include "v4p.h"

extern const V4pColor gray, marron, purple, green, cyan, black, red, blue, yellow, dark, oliver, fluo;

typedef struct v4pDisplay_s* V4pDisplayP;

extern V4pDisplayP v4pDisplayDefaultContext, v4pDisplayContext;

extern V4pCoord v4pDisplayWidth, v4pDisplayHeight;  // current display size

#define V4P_QUALITY_LOW 0
#define V4P_QUALITY_NORMAL 1
#define V4P_QUALITY_HIGH 2

Boolean v4pDisplayInit(int quality, Boolean fullscreen);
V4pDisplayP v4pDisplayNewContext(int width, int height);
V4pDisplayP v4pi_setContext(V4pDisplayP);
void v4pDisplayFreeContext(V4pDisplayP);

Boolean v4pi_start();
Boolean v4pi_slice(V4pCoord y, V4pCoord x0, V4pCoord x1, V4pColor c);
Boolean v4pi_end();
#ifdef DEBUG
void v4pi_debug(char* formatString, ...);
#else
    #define v4pi_debug (...)(0)
#endif
Boolean v4pi_error(char* s, ...);
Boolean v4pi_collide(V4pCollide i1,
                     V4pCollide i2,
                     V4pCoord py,
                     V4pCoord x1,
                     V4pCoord x2,
                     V4pPolygonP p1,
                     V4pPolygonP p2);

void v4pDisplayQuit();

#endif