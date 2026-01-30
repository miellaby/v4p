#ifndef V4PI
#define V4PI
/**
 * V4P Implementation Interface
 */

#include "v4p_ll.h"
#include "v4p.h"

// External color constants
extern const Color gray, maroon, purple, green, cyan;
extern const Color black, red, blue, yellow, dark, olive;
extern const Color fluo, white;

typedef struct v4pDisplay_s* V4pDisplayP;

// External display context and dimensions
extern V4pDisplayP v4pDisplayDefaultContext, v4pDisplayContext;
extern Coord v4pDisplayWidth, v4pDisplayHeight;  // Current display size

// Quality levels
#define V4P_QUALITY_LOW 0
#define V4P_QUALITY_NORMAL 1
#define V4P_QUALITY_HIGH 2

// Display initialization and context management
Boolean v4pDisplayInit(int quality, Boolean fullscreen);
V4pDisplayP v4pDisplayNewContext(int width, int height);
V4pDisplayP v4pi_setContext(V4pDisplayP context);
void v4pDisplayFreeContext(V4pDisplayP context);

// Display rendering functions
Boolean v4pi_start();
Boolean v4pi_slice(Coord y, Coord x0, Coord x1, Color c);
Boolean v4pi_end();

// Debug and error functions
#ifdef DEBUG
    void v4pi_debug(char* formatString, ...);
#else
    #define v4pi_debug(...)(0)
#endif
Boolean v4pi_error(char* s, ...);

// Collision detection
Boolean v4pi_collide(ICollide i1,
                     ICollide i2,
                     Coord py,
                     Coord x1,
                     Coord x2,
                     PolygonP p1,
                     PolygonP p2);

// Display cleanup
void v4pDisplayQuit();

#endif  // V4PI
