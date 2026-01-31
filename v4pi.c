/**
 * V4P Implementation for Linux + SDL
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <SDL/SDL.h>

#include "v4pi.h"
#include "lowmath.h"

/**
 * A 256 color system palette inspired from old Palm Computing Devices.
 */

 static SDL_Color palette[256]
    = { { 255, 255, 255, 0 }, { 255, 204, 255, 0 }, { 255, 153, 255, 0 }, { 255, 102, 255, 0 },
        { 255, 51, 255, 0 },  { 255, 0, 255, 0 },   { 255, 255, 204, 0 }, { 255, 204, 204, 0 },
        { 255, 153, 204, 0 }, { 255, 102, 204, 0 }, { 255, 51, 204, 0 },  { 255, 0, 204, 0 },
        { 255, 255, 153, 0 }, { 255, 204, 153, 0 }, { 255, 153, 153, 0 }, { 255, 102, 153, 0 },
        { 255, 51, 153, 0 },  { 255, 0, 153, 0 },   { 204, 255, 255, 0 }, { 204, 204, 255, 0 },
        { 204, 153, 255, 0 }, { 204, 102, 255, 0 }, { 204, 51, 255, 0 },  { 204, 0, 255, 0 },
        { 204, 255, 204, 0 }, { 204, 204, 204, 0 }, { 204, 153, 204, 0 }, { 204, 102, 204, 0 },
        { 204, 51, 204, 0 },  { 204, 0, 204, 0 },   { 204, 255, 153, 0 }, { 204, 204, 153, 0 },
        { 204, 153, 153, 0 }, { 204, 102, 153, 0 }, { 204, 51, 153, 0 },  { 204, 0, 153, 0 },
        { 153, 255, 255, 0 }, { 153, 204, 255, 0 }, { 153, 153, 255, 0 }, { 153, 102, 255, 0 },
        { 153, 51, 255, 0 },  { 153, 0, 255, 0 },   { 153, 255, 204, 0 }, { 153, 204, 204, 0 },
        { 153, 153, 204, 0 }, { 153, 102, 204, 0 }, { 153, 51, 204, 0 },  { 153, 0, 204, 0 },
        { 153, 255, 153, 0 }, { 153, 204, 153, 0 }, { 153, 153, 153, 0 }, { 153, 102, 153, 0 },
        { 153, 51, 153, 0 },  { 153, 0, 153, 0 },   { 102, 255, 255, 0 }, { 102, 204, 255, 0 },
        { 102, 153, 255, 0 }, { 102, 102, 255, 0 }, { 102, 51, 255, 0 },  { 102, 0, 255, 0 },
        { 102, 255, 204, 0 }, { 102, 204, 204, 0 }, { 102, 153, 204, 0 }, { 102, 102, 204, 0 },
        { 102, 51, 204, 0 },  { 102, 0, 204, 0 },   { 102, 255, 153, 0 }, { 102, 204, 153, 0 },
        { 102, 153, 153, 0 }, { 102, 102, 153, 0 }, { 102, 51, 153, 0 },  { 102, 0, 153, 0 },
        { 51, 255, 255, 0 },  { 51, 204, 255, 0 },  { 51, 153, 255, 0 },  { 51, 102, 255, 0 },
        { 51, 51, 255, 0 },   { 51, 0, 255, 0 },    { 51, 255, 204, 0 },  { 51, 204, 204, 0 },
        { 51, 153, 204, 0 },  { 51, 102, 204, 0 },  { 51, 51, 204, 0 },   { 51, 0, 204, 0 },
        { 51, 255, 153, 0 },  { 51, 204, 153, 0 },  { 51, 153, 153, 0 },  { 51, 102, 153, 0 },
        { 51, 51, 153, 0 },   { 51, 0, 153, 0 },    { 0, 255, 255, 0 },   { 0, 204, 255, 0 },
        { 0, 153, 255, 0 },   { 0, 102, 255, 0 },   { 0, 51, 255, 0 },    { 0, 0, 255, 0 },
        { 0, 255, 204, 0 },   { 0, 204, 204, 0 },   { 0, 153, 204, 0 },   { 0, 102, 204, 0 },
        { 0, 51, 204, 0 },    { 0, 0, 204, 0 },     { 0, 255, 153, 0 },   { 0, 204, 153, 0 },
        { 0, 153, 153, 0 },   { 0, 102, 153, 0 },   { 0, 51, 153, 0 },    { 0, 0, 153, 0 },
        { 255, 255, 102, 0 }, { 255, 204, 102, 0 }, { 255, 153, 102, 0 }, { 255, 102, 102, 0 },
        { 255, 51, 102, 0 },  { 255, 0, 102, 0 },   { 255, 255, 51, 0 },  { 255, 204, 51, 0 },
        { 255, 153, 51, 0 },  { 255, 102, 51, 0 },  { 255, 51, 51, 0 },   { 255, 0, 51, 0 },
        { 255, 255, 0, 0 },   { 255, 204, 0, 0 },   { 255, 153, 0, 0 },   { 255, 102, 0, 0 },
        { 255, 51, 0, 0 },    { 255, 0, 0, 0 },     { 204, 255, 102, 0 }, { 204, 204, 102, 0 },
        { 204, 153, 102, 0 }, { 204, 102, 102, 0 }, { 204, 51, 102, 0 },  { 204, 0, 102, 0 },
        { 204, 255, 51, 0 },  { 204, 204, 51, 0 },  { 204, 153, 51, 0 },  { 204, 102, 51, 0 },
        { 204, 51, 51, 0 },   { 204, 0, 51, 0 },    { 204, 255, 0, 0 },   { 204, 204, 0, 0 },
        { 204, 153, 0, 0 },   { 204, 102, 0, 0 },   { 204, 51, 0, 0 },    { 204, 0, 0, 0 },
        { 153, 255, 102, 0 }, { 153, 204, 102, 0 }, { 153, 153, 102, 0 }, { 153, 102, 102, 0 },
        { 153, 51, 102, 0 },  { 153, 0, 102, 0 },   { 153, 255, 51, 0 },  { 153, 204, 51, 0 },
        { 153, 153, 51, 0 },  { 153, 102, 51, 0 },  { 153, 51, 51, 0 },   { 153, 0, 51, 0 },
        { 153, 255, 0, 0 },   { 153, 204, 0, 0 },   { 153, 153, 0, 0 },   { 153, 102, 0, 0 },
        { 153, 51, 0, 0 },    { 153, 0, 0, 0 },     { 102, 255, 102, 0 }, { 102, 204, 102, 0 },
        { 102, 153, 102, 0 }, { 102, 102, 102, 0 }, { 102, 51, 102, 0 },  { 102, 0, 102, 0 },
        { 102, 255, 51, 0 },  { 102, 204, 51, 0 },  { 102, 153, 51, 0 },  { 102, 102, 51, 0 },
        { 102, 51, 51, 0 },   { 102, 0, 51, 0 },    { 102, 255, 0, 0 },   { 102, 204, 0, 0 },
        { 102, 153, 0, 0 },   { 102, 102, 0, 0 },   { 102, 51, 0, 0 },    { 102, 0, 0, 0 },
        { 51, 255, 102, 0 },  { 51, 204, 102, 0 },  { 51, 153, 102, 0 },  { 51, 102, 102, 0 },
        { 51, 51, 102, 0 },   { 51, 0, 102, 0 },    { 51, 255, 51, 0 },   { 51, 204, 51, 0 },
        { 51, 153, 51, 0 },   { 51, 102, 51, 0 },   { 51, 51, 51, 0 },    { 51, 0, 51, 0 },
        { 51, 255, 0, 0 },    { 51, 204, 0, 0 },    { 51, 153, 0, 0 },    { 51, 102, 0, 0 },
        { 51, 51, 0, 0 },     { 51, 0, 0, 0 },      { 0, 255, 102, 0 },   { 0, 204, 102, 0 },
        { 0, 153, 102, 0 },   { 0, 102, 102, 0 },   { 0, 51, 102, 0 },    { 0, 0, 102, 0 },
        { 0, 255, 51, 0 },    { 0, 204, 51, 0 },    { 0, 153, 51, 0 },    { 0, 102, 51, 0 },
        { 0, 51, 51, 0 },     { 0, 0, 51, 0 },      { 0, 255, 0, 0 },     { 0, 204, 0, 0 },
        { 0, 153, 0, 0 },     { 0, 102, 0, 0 },     { 0, 51, 0, 0 },      { 17, 17, 17, 0 },
        { 34, 34, 34, 0 },    { 68, 68, 68, 0 },    { 85, 85, 85, 0 },    { 119, 119, 119, 0 },
        { 136, 136, 136, 0 }, { 170, 170, 170, 0 }, { 187, 187, 187, 0 }, { 221, 221, 221, 0 },
        { 238, 238, 238, 0 }, { 192, 192, 192, 0 }, { 128, 0, 0, 0 },     { 128, 0, 128, 0 },
        { 0, 128, 0, 0 },     { 0, 128, 128, 0 },   { 0, 0, 0, 0 },       { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },       { 0, 0, 0, 0 },       { 0, 0, 0, 0 },       { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },       { 0, 0, 0, 0 },       { 0, 0, 0, 0 },       { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },       { 0, 0, 0, 0 },       { 0, 0, 0, 0 },       { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },       { 0, 0, 0, 0 },       { 0, 0, 0, 0 },       { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },       { 0, 0, 0, 0 },       { 0, 0, 0, 0 },       { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },       { 0, 0, 0, 0 },       { 0, 0, 0, 0 },       { 0, 0, 0, 0 } };

// Some constants linking to basic colors;
const V4pColor V4P_GRAY = 225, V4P_MAROON = 226, V4P_PURPLE = 227, V4P_GREEN = 228, V4P_CYAN = 229, V4P_BLACK = 215, V4P_RED = 125,
            V4P_BLUE = 95, V4P_YELLOW = 120, V4P_DARK = 217, V4P_OLIVE = 58, V4P_FLUO = 48, V4P_WHITE = 0;

// Default window/screen width & heigth
const V4pCoord V4P_DEFAULT_SCREEN_WIDTH = 640, V4P_DEFAULT_SCREEN_HEIGHT = 480;

// A display context
typedef struct v4pDisplay_s {
    SDL_Surface* surface;
    unsigned int width;
    unsigned int height;
} V4pDisplayS;

// Global variable hosting the default V4P contex
V4pDisplayS v4pDisplayDefaultContextS;
V4pDisplayP v4pDisplayDefaultContext = &v4pDisplayDefaultContextS;

// Variables hosting current context and related properties
V4pDisplayP v4pDisplayContext = &v4pDisplayDefaultContextS;
V4pCoord v4p_displayWidth;
V4pCoord v4p_displayHeight;
// private properties of current context
static Uint8* currentBuffer;
static int iBuffer;



/**
 * Metrics stuff
 */
static UInt32 t1;
static UInt32 laps[4] = { 0, 0, 0, 0 };
static UInt32 tlaps = 0;

// debug logging helper
#ifdef DEBUG
void v4pi_debug(char* formatString, ...) {
    va_list args;
    va_start(args, formatString);
    vprintf(formatString, args);
    va_end(args);
}
#endif

// error logging helper
Boolean v4pi_error(char* formatString, ...) {
    va_list args;
    va_start(args, formatString);
    vfprintf(stderr, formatString, args);
    va_end(args);

    return success;
}



// prepare things before V4P engine scanline loop
Boolean v4pi_start() {
    // remember start time
    t1 = SDL_GetTicks();

    // Reset buffer pointer used by v4pDisplaySplice()
    iBuffer = 0;



    // Lock before drawing if necessary
    if (SDL_MUSTLOCK(v4pDisplayContext->surface) && SDL_LockSurface(v4pDisplayContext->surface) < 0)
        return failure;
    else
        return success;
}

// finalize things after V4P engine scanline loop
Boolean v4pi_end() {
    int i;
    static int j = 0;

    // Get end time and compute average rendering time
    Uint32 t2 = SDL_GetTicks();
    tlaps -= laps[j % 4];
    tlaps += laps[j % 4] = t2 - t1;
    j++;
    if (! (j % 100))
        v4pi_debug("v4p_displayEnd, average time = %dms\n", tlaps / 4);



    // SDL locking stuff
    if (SDL_MUSTLOCK(v4pDisplayContext->surface))
        SDL_UnlockSurface(v4pDisplayContext->surface);

    // Commit graphic changes we made
    SDL_Flip(v4pDisplayContext->surface);

    return success;
}

// Draw an horizontal video slice with color 'c'
Boolean v4pi_slice(V4pCoord y, V4pCoord x0, V4pCoord x1, V4pColor c) {
    int l = x1 - x0;
    if (l <= 0)
        return success;

#ifdef DEBUG
    // Calculate expected position from iBuffer
    int last_y = iBuffer / v4pDisplayContext->surface->pitch;
    int last_x1 = iBuffer % v4pDisplayContext->surface->pitch;

    // Check if the call is consistent
    if (last_y != y || last_x1 != x0) {
        fprintf(stderr, "v4p_displaySlice: Inconsistent call sequence!\n");
        fprintf(stderr, "  Expected (from history): y=%d, x1=%d\n", last_y, last_x1);
        fprintf(stderr, "  Actual call: y=%d, x0=%d, x1=%d\n", (int) y, (int) x0, (int) x1);
        // This is a critical error - the engine's assumptions are violated
        assert(last_y == y && last_x1 == x0);
    }

#endif

    SDL_memset(&currentBuffer[iBuffer], (char) c, l);
    iBuffer += l;

#ifdef SUPPORT_UNALIGNED_WIDTH
    // Fix: Account for SDL surface pitch when moving to next scanline
    if (x1 == v4pDisplayWidth) {  // end of a scanline, add SDL's padding bytes
        iBuffer += v4pDisplayContext->surface->pitch - v4pDisplayWidth;
    }
#endif

    return success;
}

// Prepare things before the very first graphic rendering
Boolean v4pDisplayInit(int quality, Boolean fullscreen) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        v4pi_error("v4pDisplayInit failed, SDL error: '%s'\n", SDL_GetError());
        return failure;
    }

    // atexit(SDL_Quit);

    // static SDL_VideoInfo* vi = SDL_GetVideoInfo();

    // Set up a suitable video mode depending on wanted quality
    int screenWidth = V4P_DEFAULT_SCREEN_WIDTH * 2 / (3 - quality);
    int screenHeight = V4P_DEFAULT_SCREEN_HEIGHT * 2 / (3 - quality);
    SDL_Surface* screen
        = SDL_SetVideoMode(screenWidth,
                           screenHeight,
                           8 /* pixel depth */,
                           (fullscreen ? SDL_FULLSCREEN : 0) | SDL_HWSURFACE /* flags */);

    // Set a default ugly but portable 256 palette 8 bits pixel
    SDL_SetColors(screen, palette, 0, 256);

    // The default context holds the main screen/window
    const SDL_VideoInfo* info = SDL_GetVideoInfo();
    screenWidth = info->current_w;
    screenHeight = info->current_h;

    v4pDisplayDefaultContextS.surface = screen;
    v4pDisplayDefaultContextS.width = screenWidth;
    v4pDisplayDefaultContextS.height = screenHeight;
    v4pi_setContext(v4pDisplayDefaultContext);

    return success;
}

// Create a new buffer-like V4P context
V4pDisplayP v4pDisplayNewContext(int width, int height) {
    V4pDisplayP c = (V4pDisplayP) malloc(sizeof(V4pDisplayS));
    if (! c)
        return NULL;

    c->width = width;
    c->height = height;
    c->surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 8, 0, 0, 0, 0);
    return c;
}

// free a V4P context
void v4pDisplayFreeContext(V4pDisplayP c) {
    if (! c || c == v4pDisplayDefaultContext)
        return;

    SDL_FreeSurface(c->surface);
    free(c);

    // One can't let a pointer to a freed context.
    if (v4pDisplayContext == c)
        v4pDisplayContext = v4pDisplayDefaultContext;
}

// Change the current V4P context
V4pDisplayP v4pi_setContext(V4pDisplayP c) {
    v4pDisplayContext = c;
    v4p_displayWidth = c->surface->w;
    v4p_displayHeight = c->surface->h;
    currentBuffer = c->surface->pixels;
    return c;
}

// clean things before quitting
void v4pi_quit() {
    SDL_Quit();
}
