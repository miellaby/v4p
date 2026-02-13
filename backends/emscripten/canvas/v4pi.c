/**
 * V4P Implementation for Emscripten (WebAssembly)
 * This backend uses HTML5 Canvas for rendering
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>
#include <emscripten.h>
#include <emscripten/html5.h>

#include "v4pi.h"

// Global canvas context to avoid repeated DOM queries
static EM_JS(void, v4pi_cache_canvas_context, (), {
    // Cache the canvas and context globally
    window.v4pCanvas = document.getElementById('v4p-canvas');
    window.v4pCanvasContext = window.v4pCanvas ? window.v4pCanvas.getContext('2d') : null;
});

static Boolean canvas_context_cached = false;

// Default window/screen width & height
const V4pCoord V4P_DEFAULT_SCREEN_WIDTH = 640, V4P_DEFAULT_SCREEN_HEIGHT = 480;

// A display context for Emscripten
typedef struct v4pi_context_s {
    char canvas_id[64];
    unsigned int width;
    unsigned int height;
    unsigned int canvas_width;
    unsigned int canvas_height;
} V4piContext;

// Global variable hosting the default V4P context
V4piContext v4pi_defaultContextSingleton;
V4piContextP v4pi_defaultContext = &v4pi_defaultContextSingleton;

// Variables hosting current context and related properties
V4piContextP v4pi_context = &v4pi_defaultContextSingleton;
V4pCoord v4p_displayWidth;
V4pCoord v4p_displayHeight;

// Canvas context for 2D rendering
EMSCRIPTEN_WEBGL_CONTEXT_HANDLE canvas_context = 0;

/**
 * Metrics stuff
 */
static UInt32 t1;
static UInt32 laps[4] = { 0, 0, 0, 0 };
static UInt32 tlaps = 0;

// Prepare things before V4P engine scanline loop
Boolean v4pi_start() {
    // remember start time
    t1 = emscripten_get_now();
    return success;
}

// finalize things after V4P engine scanline loop
Boolean v4pi_end() {
    int i;
    static int j = 0;

    // Get end time and compute average rendering time
    UInt32 t2 = emscripten_get_now();
    tlaps -= laps[j % 4];
    tlaps += laps[j % 4] = t2 - t1;
    j++;
    if (!(j % 100))
        v4pi_debug("v4p_displayEnd, average time = %dms\n", tlaps / 4);

    return success;
}

// Draw an horizontal video slice with color 'c'
Boolean v4pi_slice(V4pCoord y, V4pCoord x0, V4pCoord x1, V4pColor c) {
    int l = x1 - x0;
    if (l <= 0)
        return success;

    // Cache canvas context on first call to avoid repeated DOM queries
    if (!canvas_context_cached) {
        v4pi_cache_canvas_context();
        canvas_context_cached = true;
    }

    // Convert V4P color to HTML color
    UInt8 r = V4P_PALETTE_R(c);
    UInt8 g = V4P_PALETTE_G(c);
    UInt8 b = V4P_PALETTE_B(c);
    
    // Use cached global variables for much better performance
    EM_ASM_({
        if (window.v4pCanvasContext) {
            window.v4pCanvasContext.fillStyle = 'rgb(' + $0 + ',' + $1 + ',' + $2 + ')';
            window.v4pCanvasContext.fillRect($3, $4, $5, 1);
        }
    }, r, g, b, x0, y, l);

    return success;
}

// Prepare things before the very first graphic rendering
Boolean v4pi_init(int quality, Boolean fullscreen) {
    // Set up canvas dimensions based on quality
    int screenWidth = V4P_DEFAULT_SCREEN_WIDTH * 2 / (3 - quality);
    int screenHeight = V4P_DEFAULT_SCREEN_HEIGHT * 2 / (3 - quality);

    // Use a fixed canvas ID to match the template
    strncpy(v4pi_defaultContextSingleton.canvas_id, "v4p-canvas", sizeof(v4pi_defaultContextSingleton.canvas_id));

    // Create canvas element using Emscripten
    EM_ASM_({
        var canvas = document.createElement('canvas');
        canvas.id = UTF8ToString($0);
        canvas.width = $1;
        canvas.height = $2;
        canvas.style.border = 'none';
        canvas.style.display = 'block';
        canvas.style.margin = '0 auto';
        document.body.appendChild(canvas);
    }, v4pi_defaultContextSingleton.canvas_id, screenWidth, screenHeight);

    // The default context holds the main canvas
    v4pi_defaultContextSingleton.width = screenWidth;
    v4pi_defaultContextSingleton.height = screenHeight;
    v4pi_defaultContextSingleton.canvas_width = screenWidth;
    v4pi_defaultContextSingleton.canvas_height = screenHeight;
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
    c->canvas_width = width;
    c->canvas_height = height;
    
    // Create a unique canvas ID for this context
    snprintf(c->canvas_id, sizeof(c->canvas_id), "v4p_canvas_%p", (void*)c);

    // Create canvas element
    EM_ASM_({
        var canvas = document.createElement('canvas');
        canvas.id = UTF8ToString($0);
        canvas.width = $1;
        canvas.height = $2;
        canvas.style.display = 'none'; // Hidden by default
        document.body.appendChild(canvas);
    }, c->canvas_id, width, height);

    return c;
}

// free a V4P context
void v4pi_destroyContext(V4piContextP c) {
    if (! c || c == v4pi_defaultContext)
        return;

    // Remove the canvas element
    EM_ASM_({
        var canvas = document.getElementById(UTF8ToString($0));
        if (canvas && canvas.parentNode) {
            canvas.parentNode.removeChild(canvas);
        }
    }, c->canvas_id);

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
    // No special cleanup needed for Emscripten
}