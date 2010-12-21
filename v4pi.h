#ifndef V4PI
#define V4PI
/* V4P Implementation IF
**
*/

#include "v4p_ll.h"
#include "v4p.h"

extern const Color
   gray, marron, purple, green, cyan,
   black, red, blue, yellow, dark, oliver,
   fluo;

typedef struct v4pDisplay_s* V4pDisplayP;

extern V4pDisplayP v4pDisplayDefaultContext, v4pDisplayContext;

extern Coord v4pDisplayWidth, v4pDisplayHeight; // current display size

#define V4P_QUALITY_LOW 0
#define V4P_QUALITY_NORMAL 1
#define V4P_QUALITY_HIGH 2

Boolean v4pDisplayInit(int quality, Boolean fullscreen);
V4pDisplayP v4pDisplayNewContext(int width, int height);
V4pDisplayP v4pDisplaySetContext(V4pDisplayP);
void    v4pDisplayFreeContext(V4pDisplayP);

Boolean v4pDisplayStart();
Boolean v4pDisplaySlice(Coord y, Coord x0, Coord x1, Color c);
Boolean v4pDisplayEnd();
#ifdef DEBUG
void    v4pDisplayDebug(char *formatString,...);
#else
#define v4pDisplayDebug(...) (0)
#endif
Boolean v4pDisplayError(char *s,...);
Boolean v4pDisplayCollide(ICollide i1, ICollide i2, Coord py, Coord x1, Coord x2, PolygonP p1, PolygonP p2);

void    v4pDisplayQuit();


#endif
