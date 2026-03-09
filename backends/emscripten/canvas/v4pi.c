/**
 * V4P Implementation for Emscripten (WebAssembly)
 * This backend uses HTML5 Canvas for rendering
 */
#include "v4pi.h"
#include "v4p_platform.h"
#include "v4p_trace.h"
#include "v4p_color.h"
#include <emscripten.h>
#include <emscripten/html5.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>

// Global canvas context to avoid repeated DOM queries
static EM_JS(void, v4pi_cache_canvas_context, (), {
    // Cache the canvas and context globally
    window.v4pCanvas = document.getElementById('v4p-canvas');
    window.v4pCanvasContext = window.v4pCanvas ? window.v4pCanvas.getContext('2d') : null;
});

static bool canvas_context_cached = false;

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
static uint32_t t1;
static uint32_t laps[4] = { 0, 0, 0, 0 };

// Prepare things before V4P engine scanline loop
int v4pi_start() {
    // remember start time
    t1 = emscripten_get_now();
    return success;
}

// finalize things after V4P engine scanline loop
int v4pi_end() {
    int i;
    static int j = 0;

    // Get end time and compute average rendering time
    uint32_t t2 = emscripten_get_now();
    laps[j++ % 4] = t2 - t1;
    if (! (j % 100)) v4p_trace(RENDER, "render time = %.1fms\n", (laps[0] + laps[1] + laps[2] + laps[3]) / 4.0);
    return success;
}

// Draw an horizontal video slice with color 'c'
int v4pi_slice(V4pCoord y, V4pCoord x0, V4pCoord x1, V4pColor c) {
    int l = x1 - x0;
    if (l <= 0)
        return success;

    // Cache canvas context on first call to avoid repeated DOM queries
    if (!canvas_context_cached) {
        v4pi_cache_canvas_context();
        canvas_context_cached = true;
    }

    // Convert V4P color to HTML color
    uint8_t r = V4P_PALETTE_R(c);
    uint8_t g = V4P_PALETTE_G(c);
    uint8_t b = V4P_PALETTE_B(c);
    
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
int v4pi_init(int quality, bool fullscreen) {
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