/**
 * V4P Implementation for Linux + SDL
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <SDL/SDL.h>

#include "v4pi.h"

// SDL Color palette
static SDL_Color sdlColors[256];

// Default window/screen width & heigth
const V4pCoord V4P_DEFAULT_SCREEN_WIDTH = 640, V4P_DEFAULT_SCREEN_HEIGHT = 480;

// A display context
typedef struct v4pi_context_s {
    SDL_Surface* surface;
    unsigned int width;
    unsigned int height;
} V4piContext;

// Global variable hosting the default V4P contex
V4piContext v4pi_defaultContextSingleton;
V4piContextP v4pi_defaultContext = &v4pi_defaultContextSingleton;

// Variables hosting current context and related properties
V4piContextP v4pi_context = &v4pi_defaultContextSingleton;
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

static void init_palette() {
    for (int i = 0; i < 256; i++) {
        sdlColors[i].r = V4P_PALETTE_R(i);
        sdlColors[i].g = V4P_PALETTE_G(i);
        sdlColors[i].b = V4P_PALETTE_B(i);
        sdlColors[i].unused = 0;
    }
}

// prepare things before V4P engine scanline loop
Boolean v4pi_start() {
    // remember start time
    t1 = SDL_GetTicks();

    // Reset buffer pointer used by v4pDisplaySplice()
    iBuffer = 0;

    // Lock before drawing if necessary
    if (SDL_MUSTLOCK(v4pi_context->surface) && SDL_LockSurface(v4pi_context->surface) < 0)
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
    if (SDL_MUSTLOCK(v4pi_context->surface))
        SDL_UnlockSurface(v4pi_context->surface);

    // Commit graphic changes we made
    SDL_Flip(v4pi_context->surface);

    return success;
}

// Draw an horizontal video slice with color 'c'
Boolean v4pi_slice(V4pCoord y, V4pCoord x0, V4pCoord x1, V4pColor c) {
    int l = x1 - x0;
    if (l <= 0)
        return success;

#ifdef DEBUG
    // Calculate expected position from iBuffer
    int last_y = iBuffer / v4pi_context->surface->pitch;
    int last_x1 = iBuffer % v4pi_context->surface->pitch;

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
    if (x1 == v4p_displayWidth) {  // end of a scanline, add SDL's padding bytes
        iBuffer += v4pi_context->surface->pitch - v4p_displayWidth;
    }
#endif

    return success;
}

// Prepare things before the very first graphic rendering
Boolean v4pi_init(int quality, Boolean fullscreen) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        v4p_error("v4pi_init failed, SDL error: '%s'\n", SDL_GetError());
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

    // Set the ugly 256 colors palette
    init_palette();
    SDL_SetColors(screen, sdlColors, 0, 256);

    // The default context holds the main screen/window
    const SDL_VideoInfo* info = SDL_GetVideoInfo();
    screenWidth = info->current_w;
    screenHeight = info->current_h;

    v4pi_defaultContextSingleton.surface = screen;
    v4pi_defaultContextSingleton.width = screenWidth;
    v4pi_defaultContextSingleton.height = screenHeight;
    v4pi_setContext(v4pi_defaultContext);

    return success;
}

// Create a new buffer-like V4P context
V4piContextP v4pi_newContext(int width, int height) {
    V4piContextP c = (V4piContextP) malloc(sizeof(V4piContext));
    if (! c)
        return NULL;

    c->width = width;
    c->height = height;
    c->surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 8, 0, 0, 0, 0);
    return c;
}

// free a V4P context
void v4pi_destroyContext(V4piContextP c) {
    if (! c || c == v4pi_defaultContext)
        return;

    SDL_FreeSurface(c->surface);
    free(c);

    // One can't let a pointer to a freed context.
    if (v4pi_context == c)
        v4pi_context = v4pi_defaultContext;
}

// Change the current V4P context
V4piContextP v4pi_setContext(V4piContextP c) {
    v4pi_context = c;
    v4p_displayWidth = c->surface->w;
    v4p_displayHeight = c->surface->h;
    currentBuffer = c->surface->pixels;
    return c;
}

// clean things before quitting
void v4pi_destroy() {
    SDL_Quit();
}
