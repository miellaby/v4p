/**
 * V4P Implementation for Linux + X
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/times.h>
#include <string.h>

#include <X11/Xutil.h>
#include "v4pi.h"

typedef struct v4pi_context_s {
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
} V4piContext;

Display* currentDisplay;
Window currentWindow;
int currentScreen;
GC currentGC;
// Pixmap    currentPixmap;
int currentDepth;
XImage* currentImage;
char* currentBuffer;

static unsigned long xColors[256];  // X Colors

static char* applicationName = "v4pX";
static char* applicationClass = "V4pX";
static char* fakeArgv[] = { "v4pX" };
// Default window/screen width & heigth
V4pCoord V4P_DEFAULT_SCREEN_WIDTH = 640, V4P_DEFAULT_SCREEN_HEIGHT = 480;

// Default V4P context
V4piContext v4pi_defaultContextSingleton;
V4piContextP v4pi_defaultContext = &v4pi_defaultContextSingleton;

// Variables hosting current context and related properties
V4piContextP v4pi_context = &v4pi_defaultContextSingleton;
V4pCoord v4p_displayWidth;
V4pCoord v4p_displayHeight;

// private properties of current context
static const int borderWidth = 1;
static int iBuffer;

// Data buffer
static XGCValues values;

/**
 * Metrics stuff
 */

static Int32 t1;
static Int32 laps[4] = { 0, 0, 0, 0 };
static Int32 tlaps = 0;

static Int32 getTicks() {
    static struct tms buf;
    static int clk_ticks = 0;
    if (! clk_ticks) {
        clk_ticks = sysconf(_SC_CLK_TCK);
        printf("clk_ticks = %d\n", clk_ticks);
    }

    Int32 t = times(&buf) * 1000 / clk_ticks;
    return t;
}

// prepare things before V4P engine scanline loop
Boolean v4pi_start() {
    // remember start time
    t1 = getTicks();

    // Reset buffer pointer used by v4pDisplaySplice()
    iBuffer = 0;

    return success;
}

Boolean v4pi_end() {
    // Get end time and compute average rendering time
    static int j = 0;
    Int32 t2 = getTicks();
    tlaps -= laps[j % 4];
    tlaps += laps[j % 4] = t2 - t1;
    j++;
    if (! (j % 100))
        v4pi_debug("v4p_displayEnd, average time = %dms\n", tlaps / 4);

    // Commit graphic changes we made
    XPutImage(currentDisplay,
              currentWindow /* currentPixmap */,
              currentGC,
              currentImage,
              0,
              0,
              0,
              0,
              v4p_displayWidth,
              v4p_displayWidth);
    // XCopyArea(currentDisplay, currentPixmap, currentWindow, currentGC, 0, 0,
    // v4p_displayWidth, v4p_displayWidth, 0, 0);
    XFlush(currentDisplay);
    return success;
}

// Draw an horizontal video slice with color 'c'
Boolean v4pi_slice(V4pCoord y, V4pCoord x0, V4pCoord x1, V4pColor c) {
    int l = x1 - x0;
    if (l <= 0)
        return success;
    unsigned long pixel = xColors[c];

    if (currentDepth == 8) {
        memset(&currentBuffer[iBuffer], pixel, l);
        iBuffer += l;
    } else if (currentDepth == 16) {
        while (l--) {
            currentBuffer[iBuffer++] = pixel & 0xFF;
            currentBuffer[iBuffer++] = (pixel >> 8) & 0xFF;
        }
    } else if (currentDepth >= 24) {
        while (l--) {
            *(long*) (&currentBuffer[iBuffer += 4]) = pixel;
        }
    }

    return success;
}

// Create and "map" a window
static Boolean createWindow(V4piContextP vd, int width, int height) {
    Display* d = vd->d;
    int s = vd->s;
    Window w;
    GC gc;

    /* Miscellaneous X variables */
    XSizeHints* size_hints;
    XWMHints* wm_hints;
    XClassHint* class_hints;
    XTextProperty windowName, iconName;

    w = /* create window */
        XCreateSimpleWindow(d,
                            RootWindow(d, s),
                            10,
                            10,
                            width,
                            height,
                            borderWidth,
                            BlackPixel(d, s),
                            WhitePixel(d, s));

    /* Allocate memory */
    if (! (size_hints = XAllocSizeHints()) || ! (wm_hints = XAllocWMHints())
        || ! (class_hints = XAllocClassHint())) {
        v4p_error("couldn't allocate memory.\n");
    }

    /*  Set hints for window manager */
    if (XStringListToTextProperty(&applicationName, 1, &windowName) == 0
        || XStringListToTextProperty(&applicationName, 1, &iconName) == 0) {
        v4p_error("xlib structure allocation failed.\n");
        return failure;
    }

    size_hints->flags = PPosition | PSize | PMinSize;
    size_hints->min_width = width;
    size_hints->min_height = height;

    wm_hints->flags = StateHint | InputHint;
    wm_hints->initial_state = NormalState;
    wm_hints->input = True;

    class_hints->res_name = applicationName;
    class_hints->res_class = applicationClass;

    XSetWMProperties(d,
                     w,
                     &windowName,
                     &iconName,
                     (char**) &fakeArgv,
                     0,
                     size_hints,
                     wm_hints,
                     class_hints);

    /*  Choose which events we want to handle  */
    XSelectInput(d,
                 w,
                 ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask
                     | ButtonMotionMask | PointerMotionHintMask | StructureNotifyMask);

    gc = /* create graphic context */
        XCreateGC(d, w, 0, &values);
    XSetForeground(d, gc, BlackPixel(d, s));

    int depth = DefaultDepth(d, s);
    v4pi_debug("depth is %d\n", depth);

    // Pixmap p = XCreatePixmap(d, w, width, height, 8 /* not default depth */);

    char* buffer = (char*) malloc(width * height * (depth <= 8 ? 1 : 4));
    if (! buffer) {
        v4p_error("malloc failed \n");
        return failure;
    }
    XImage* i = XCreateImage(d, DefaultVisual(d, s), depth, ZPixmap, 0, buffer, width, height, 8, 0);
    XInitImage(i);
    vd->width = width;
    vd->height = height;
    vd->depth = depth;
    vd->w = w;
    vd->gc = gc;
    // vd->p = p;
    vd->b = buffer;
    vd->i = i;
    /*  Display Window  */
    XMapWindow(d, w);

    return success;
}

// Prepare things before the very first graphic rendering
Boolean v4pi_init(int quality, Boolean fullscreen) {
    int rc = success;

    /* connect to X server */
    Display* d = XOpenDisplay(NULL);
    if (d == NULL) {
        v4p_error("Cannot open display\n");
        exit(EXIT_FAILURE);
    }

    // Prepare "tablette"
    int s = DefaultScreen(d);
    Colormap cmap = DefaultColormap(d, s);

    v4pi_defaultContextSingleton.d = d;
    v4pi_defaultContextSingleton.s = s;

    /* Get screen size from display structure macro */
    V4P_DEFAULT_SCREEN_WIDTH = DisplayWidth(d, s);
    V4P_DEFAULT_SCREEN_HEIGHT = DisplayHeight(d, s);
    int winWidth = V4P_DEFAULT_SCREEN_WIDTH * 2 / (3 - quality);
    int winHeight = V4P_DEFAULT_SCREEN_HEIGHT * 2 / (3 - quality);

    rc |= createWindow(v4pi_defaultContext, winWidth, winHeight);

    XColor c;
    int i, rcx;
    for (i = 0; i < 256; i++) {
        c.red = (unsigned short) V4P_PALETTE_R(i) << 8;
        c.green = (unsigned short) V4P_PALETTE_G(i) << 8;
        c.blue = (unsigned short) V4P_PALETTE_B(i) << 8;
        // v4pi_debug ("color allocation %02X %02X %02X %08lX\n", (int)c.V4P_RED,
        // (int)c.V4P_GREEN, (int)c.V4P_BLUE, (unsigned long)c.pixel);
        if (! (rcx = XAllocColor(d, cmap, &c))) {
            v4p_error("Can't allocate color\n");
            exit(EXIT_FAILURE);
        }
        // v4pi_debug ("color allocation %02X %02X %02X %08lX\n", (int)c.V4P_RED,
        // (int)c.V4P_GREEN, (int)c.V4P_BLUE, (unsigned long)c.pixel);
        xColors[i] = c.pixel;
        // printf("%d %02X %02X %02X %08lX\n", rcx, (int)c.V4P_RED, (int)c.V4P_GREEN,
        // (int)c.V4P_BLUE, (unsigned long)c.pixel);
    }

    v4pi_setContext(v4pi_defaultContext);

    return rc;
}

// Create a second window as V4P context
V4piContextP v4pi_newContext(int width, int height) {
    V4piContextP c = (V4piContextP) malloc(sizeof(V4piContext));
    if (! c) {
        v4p_error("malloc failed \n");
        return 0;
    }
    c->w = v4pi_defaultContextSingleton.w;
    c->d = v4pi_defaultContextSingleton.d;
    c->s = v4pi_defaultContextSingleton.s;
    c->gc = v4pi_defaultContextSingleton.gc;
    c->depth = v4pi_defaultContextSingleton.depth;
    char* buffer = (char*) malloc(width * height * (c->depth <= 8 ? 1 : 4));
    if (! buffer) {
        v4p_error("malloc failed \n");
        return 0;
    }
    c->b = buffer;

    XImage* i = XCreateImage(c->d,
                             DefaultVisual(c->d, c->s),
                             c->depth,
                             ZPixmap,
                             0,
                             buffer,
                             width,
                             height,
                             8,
                             0);
    XInitImage(i);

    c->i = i;
    c->width = width;
    c->height = height;

    return c;
}

// free a V4P context
void v4pi_destroyContext(V4piContextP c) {
    if (! c || c == v4pi_defaultContext)
        return;
    XDestroyImage(c->i);
    free(c);

    // One can't let a pointer to a freed context.
    if (v4pi_context == c)
        v4pi_context = v4pi_defaultContext;
}

// Change the current V4P context
V4piContextP v4pi_setContext(V4piContextP c) {
    v4pi_context = c;
    v4p_displayWidth = c->width;
    v4p_displayHeight = c->height;
    currentDisplay = c->d;
    currentWindow = c->w;
    currentScreen = c->s;
    currentGC = c->gc;
    // currentPixmap = c->p;
    currentDepth = c->depth;
    currentBuffer = c->b;
    currentImage = c->i;
    return c;
}

// clean things before quitting
void v4pi_destroy() {
    /* close connection to server */
    XFreeGC(v4pi_defaultContext->d, v4pi_defaultContext->gc);
    XCloseDisplay(v4pi_defaultContext->d);
}
