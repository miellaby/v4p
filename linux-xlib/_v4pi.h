#include <X11/Xlib.h>
#include "v4pi.h"

typedef struct v4pDisplay_s {
    Display* d;
    int s;
    Window w;
    GC gc;
    //  Pixmap   p;
    XImage* i;
    char* b;
    unsigned int width;
    unsigned int height;
    int depth;
} V4pDisplayS;

extern Display* currentDisplay;
extern Window currentWindow;
extern int currentScreen;
extern GC currentGC;
// Pixmap    currentPixmap;
extern int currentDepth;
extern XImage* currentImage;
extern char* currentBuffer;
