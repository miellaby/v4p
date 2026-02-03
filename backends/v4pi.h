#ifndef V4PI
#define V4PI
/**
 * V4P Implementation I/F
 * Depends on platform/backend
 */

#include "v4p_ll.h"
#include "v4p.h"
typedef struct v4pi_context_s* V4piContextP;

/** default display context */
extern V4piContextP v4pi_defaultContext;

/** current display context */
extern V4piContextP v4pi_context;

/** Initialize the implementation and a default context */
Boolean v4pi_init(int quality, Boolean fullscreen);

/** To cleanup resources */
void v4pi_destroy();

/*
 * Contexts
*/
/** Create a secondary context */
V4piContextP v4pi_newContext(int width, int height);

/** Change the current context to a secondary context or v4pi_defaultContext */
V4piContextP v4pi_setContext(V4piContextP context);

/** destroy a display context as created by v4pi_newContext */
void v4pi_destroyContext(V4piContextP context);

/*
 * Scan-Line Slice/Span Rendition 
*/

// Prepare before first scanline rendering
Boolean v4pi_start();

// Render Span/Slice
Boolean v4pi_slice(V4pCoord y, V4pCoord x0, V4pCoord x1, V4pColor c);

// Finalize after last scanline rendered
Boolean v4pi_end();

#endif  // V4PI
