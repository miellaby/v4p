/**
 * V4P Implementation for Emscripten Bitmap Backend
 * Creates a bitmap in memory and renders to it
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

// Default display dimensions
const V4pCoord V4P_DEFAULT_SCREEN_WIDTH = 640;
const V4pCoord V4P_DEFAULT_SCREEN_HEIGHT = 480;

// Bitmap context structure
// Note: canvas_id must be first for g4pi.c to access it
typedef struct v4pi_context_s {
    char canvas_id[64];  // Must be first field for g4pi.c access
    unsigned int width;
    unsigned int height;
    uint32_t* bitmap;  // Pointer to bitmap memory (RGBA format)
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE canvas_context;
} V4piContext;

// Global variables
V4piContext v4pi_defaultContextSingleton;
V4piContextP v4pi_defaultContext = &v4pi_defaultContextSingleton;
V4piContextP v4pi_context = &v4pi_defaultContextSingleton;
V4pCoord v4p_displayWidth = V4P_DEFAULT_SCREEN_WIDTH;
V4pCoord v4p_displayHeight = V4P_DEFAULT_SCREEN_HEIGHT;

static uint32_t t1;
static uint32_t laps[4] = { 0, 0, 0, 0 };

// Initialize the bitmap backend
int v4pi_init(int quality, bool fullscreen) {
    // Calculate display dimensions based on quality
    int screenWidth = V4P_DEFAULT_SCREEN_WIDTH * 2 / (3 - quality);
    int screenHeight = V4P_DEFAULT_SCREEN_HEIGHT * 2 / (3 - quality);

    // Allocate bitmap memory (RGBA format)
    size_t bitmap_size = screenWidth * screenHeight * sizeof(uint32_t);
    v4pi_defaultContext->bitmap = (uint32_t*)malloc(bitmap_size);
    if (!v4pi_defaultContext->bitmap) {
        v4p_debug("Failed to allocate bitmap memory\n");
        return failure;
    }

    // Clear bitmap to transparent black
    memset(v4pi_defaultContext->bitmap, 0, bitmap_size);

    // Set context properties
    v4pi_defaultContext->width = screenWidth;
    v4pi_defaultContext->height = screenHeight;
    v4p_displayWidth = screenWidth;
    v4p_displayHeight = screenHeight;

    // Create canvas for display
    snprintf(v4pi_defaultContext->canvas_id, sizeof(v4pi_defaultContext->canvas_id), "v4p-bitmap-canvas");

    EM_ASM_({
        var canvas = document.createElement('canvas');
        canvas.id = UTF8ToString($0);
        canvas.width = $1;
        canvas.height = $2;
        canvas.style.border = 'none';
        canvas.style.display = 'block';
        canvas.style.margin = '0 auto';
        
        // Place canvas in the canvas-container div from the template
        var container = document.querySelector('.canvas-container');
        if (container) {
            container.appendChild(canvas);
        } else {
            // Fallback to body if container not found
            document.body.appendChild(canvas);
        }
    }, v4pi_defaultContext->canvas_id, screenWidth, screenHeight);

    return success;
}

// Start rendering - clear bitmap
int v4pi_start() {
    // remember start time
    t1 = v4p_getTicks();

    // Clear bitmap to transparent black
    size_t bitmap_size = v4pi_context->width * v4pi_context->height * sizeof(uint32_t);
    memset(v4pi_context->bitmap, 0, bitmap_size);
    return success;
}

// Draw a slice to the bitmap
int v4pi_slice(V4pCoord y, V4pCoord x0, V4pCoord x1, V4pColor c) {
    if (x1 <= x0 || y < 0 || y >= v4pi_context->height) {
        return success;
    }

    // Convert V4P color to RGBA
    // ImageData expects bytes in RGBA order: [R, G, B, A]
    // On little-endian systems, uint32_t 0xAABBGGRR is stored as bytes [RR, GG, BB, AA]
    // So to get [R, G, B, A] in memory, we need: (A << 24) | (B << 16) | (G << 8) | R
    uint8_t r = V4P_PALETTE_R(c);
    uint8_t g = V4P_PALETTE_G(c);
    uint8_t b = V4P_PALETTE_B(c);
    
    // Create RGBA value with correct byte order for ImageData
    uint32_t rgba = (0xFF << 24) | (b << 16) | (g << 8) | r;

    // Write to bitmap (clamp x1 to display width)
    V4pCoord end_x = (x1 > v4pi_context->width) ? v4pi_context->width : x1;

    // Write the slice to the bitmap
    uint32_t* row_ptr = v4pi_context->bitmap + (y * v4pi_context->width);
    for (V4pCoord x = x0; x < end_x; x++) {
        row_ptr[x] = rgba;
    }

    return success;
}

// Finalize rendering and display bitmap
int v4pi_end() {
    static int j = 0;

    // Transfer bitmap to canvas
    EM_ASM_({
        var canvas = document.getElementById(UTF8ToString($0));
        if (canvas) {
            var ctx = canvas.getContext('2d');
            var imageData = new ImageData(new Uint8ClampedArray(HEAPU8.buffer, $3, $4), $1, $2);
            ctx.putImageData(imageData, 0, 0);
        }
    }, v4pi_context->canvas_id, v4pi_context->width, v4pi_context->height,
       v4pi_context->bitmap, v4pi_context->width * v4pi_context->height * 4);

    uint32_t t2 = v4p_getTicks();
    laps[j++ % 4] = t2 - t1;
    if (! (j % 100)) v4p_trace(RENDER, "render time = %.1fms\n", (laps[0] + laps[1] + laps[2] + laps[3]) / 4.0);
    return success;
}

// Context management functions
V4piContextP v4pi_newContext(int width, int height) {
    V4piContextP c = (V4piContextP)malloc(sizeof(V4piContext));
    if (!c) return NULL;

    c->width = width;
    c->height = height;
    c->bitmap = (uint32_t*)malloc(width * height * sizeof(uint32_t));
    if (!c->bitmap) {
        free(c);
        return NULL;
    }

    // Create unique canvas ID
    snprintf(c->canvas_id, sizeof(c->canvas_id), "v4p_bitmap_canvas_%p", (void*)c);

    // Create hidden canvas
    EM_ASM_({
        var canvas = document.createElement('canvas');
        canvas.id = UTF8ToString($0);
        canvas.width = $1;
        canvas.height = $2;
        canvas.style.display = 'none';
        document.body.appendChild(canvas);
    }, c->canvas_id, width, height);

    return c;
}

V4piContextP v4pi_setContext(V4piContextP c) {
    v4pi_context = c;
    v4p_displayWidth = c->width;
    v4p_displayHeight = c->height;
    return c;
}

void v4pi_destroyContext(V4piContextP c) {
    if (!c || c == v4pi_defaultContext) return;

    if (c->bitmap) free(c->bitmap);

    EM_ASM_({
        var canvas = document.getElementById(UTF8ToString($0));
        if (canvas && canvas.parentNode) {
            canvas.parentNode.removeChild(canvas);
        }
    }, c->canvas_id);

    free(c);
    if (v4pi_context == c) v4pi_context = v4pi_defaultContext;
}

void v4pi_destroy() {
    // Cleanup will be handled by v4pi_destroyContext
}