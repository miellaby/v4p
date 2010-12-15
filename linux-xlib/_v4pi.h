#include <X11/Xlib.h>
#include "v4pi.h"

typedef struct v4pDisplay_s {
  Display *d;
  int      s;
  Window   w;
  GC       gc;
  unsigned int width;
  unsigned int height;
} V4pDisplayS;


Display*  currentDisplay;
Window    currentWindow;
int       currentScreen;
GC        currentGC;
