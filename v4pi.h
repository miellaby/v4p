#ifndef V4PI
#define V4PI
/**
 * V4P Implementation Interface
 */

#include "v4p_ll.h"
#include "v4p.h"

// External color constants
extern const V4pColor V4P_GRAY, V4P_MAROON, V4P_PURPLE, V4P_GREEN, V4P_CYAN;
extern const V4pColor V4P_BLACK, V4P_RED, V4P_BLUE, V4P_YELLOW, V4P_DARK, V4P_OLIVE;
extern const V4pColor V4P_FLUO, V4P_WHITE;

typedef struct v4pDisplay_s* V4pDisplayP;

// External display context and dimensions
extern V4pDisplayP v4pDisplayDefaultContext, v4pDisplayContext;
extern V4pCoord v4p_displayWidth, v4p_displayHeight;  // Current display size

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
Boolean v4pi_slice(V4pCoord y, V4pCoord x0, V4pCoord x1, V4pColor c);
Boolean v4pi_end();

// Debug and error functions
#ifdef DEBUG
    void v4pi_debug(char* formatString, ...);
#else
    #define v4pi_debug(...)(0)
#endif
Boolean v4pi_error(char* s, ...);



// Display cleanup
void v4pi_quit();

#endif  // V4PI
