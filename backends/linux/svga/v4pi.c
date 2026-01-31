/**
 * V4P Implementation for Linux + SDL
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/times.h>
#include <vga.h>
#include "v4pi.h"

// Default window/screen width & heigth
const V4pCoord V4P_DEFAULT_SCREEN_WIDTH = 640, V4P_DEFAULT_SCREEN_HEIGHT = 480;

// A display context
typedef struct v4pi_context_s {
    // TODO manage offscreen buffer
    unsigned int width;
    unsigned int height;
} V4piContext;

// Global variable hosting the default V4P contex
V4piContext v4pi_defaultContextSingleton;
const V4piContextP v4pi_defaultContext = &v4pi_defaultContextSingleton;

// Variables hosting current context and related properties
V4piContextP v4pi_context = &v4pi_defaultContextSingleton;
V4pCoord v4p_displayWidth;
V4pCoord v4p_displayHeight;

// prepare things before V4P engine scanline loop
Boolean v4pi_start() {
    // remember start time
    return success;
}

// finalize things after V4P engine scanline loop
Boolean v4pi_end() {
    return success;
}

// Draw an horizontal video slice with color 'c'
Boolean v4pi_slice(V4pCoord y, V4pCoord x0, V4pCoord x1, V4pColor c) {
    int l = x1 - x0;
    if (l <= 0)
        return success;
    vga_setcolor(c);
    vga_drawline(x0, y, x1, y);
    return success;
}

// Prepare things before the very first graphic rendering
Boolean v4pi_init(int quality, Boolean fullscreen) {
    // Initialize Svgalib
    vga_init();

    // Set up a suitable video mode depending on wanted quality
    int screenWidth = V4P_DEFAULT_SCREEN_WIDTH * 2 / (3 - quality);
    int screenHeight = V4P_DEFAULT_SCREEN_HEIGHT * 2 / (3 - quality);
    v4pi_debug("v4pi_init, wanted resolution is %dx%d\n", screenWidth, screenHeight);
    vga_setmode(screenWidth == 640 && screenHeight == 480
                    ? G640x480x256
                    : (screenWidth > 640 ? G800x600x256 : G320x200x256));

    // Set a default ugly but portable 256 palette 8 bits pixel
    vga_setpalvec(0, 256, palette);

    // The default context holds the main screen/window
    // TODO get actual screen geometry
    vga_modeinfo* i = vga_getmodeinfo(vga_getcurrentmode());

    v4pi_defaultContextSingleton.width = i->width;
    v4pi_defaultContextSingleton.height = i->height;
    v4pi_debug("Default Context Width x height = %d x %d\n",
               v4pi_defaultContextSingleton.width,
               v4pi_defaultContextSingleton.height);
    v4pi_setContext(v4pi_defaultContext);

    return success;
}

// Create a new buffer-like V4P context
V4piContextP v4pi_newContext(int width, int height) {
    V4piContextP c = (V4piContextP) malloc(sizeof(V4piContext));
    if (! c)
        return NULL;
    // TODO offscren buffer
    c->width = width;
    c->height = height;
    return c;
}

// free a V4P context
void v4pi_destroyContext(V4piContextP c) {
    if (! c || c == v4pi_defaultContext)
        return;

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
    return c;
}

// clean things before quitting
void v4pi_destroy() {
    vga_setmode(TEXT);
    v4pi_debug("w=%d h=%d\n", v4pi_defaultContextSingleton.width, v4pi_defaultContextSingleton.height);
}
