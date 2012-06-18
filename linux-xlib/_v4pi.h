#include <X11/Xlib.h>
#include "v4pi.h"

typedef struct v4pDisplay_s {
  Display *d;
  int      s;
  Window   w;
  GC       gc;
//  Pixmap   p;
  XImage*  i;
  char*    b;
  unsigned int width;
  unsigned int height;
  int      depth;
} V4pDisplayS;


Display*  currentDisplay;
Window    currentWindow;
int       currentScreen;
GC        currentGC;
// Pixmap    currentPixmap;
int       currentDepth;
XImage*   currentImage;
char*     currentBuffer;
