/**
 * V4P Implementation for Linux + libcaca (ASCII art backend)
 * 
 * This backend uses libcaca to render vector graphics as ASCII art in a terminal.
 * It uses direct libcaca API calls for better performance and quality.
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>
#include <caca.h>

#include "v4pi.h"
#include "v4p_color.h"

// Default window/screen dimensions in characters
const V4pCoord V4P_DEFAULT_SCREEN_WIDTH = 80, V4P_DEFAULT_SCREEN_HEIGHT = 25;

// A display context for libcaca
typedef struct v4pi_context_s {
    caca_display_t* display;
    caca_canvas_t* canvas;
    caca_dither_t* dither;
    unsigned int width;      // Width in pixels
    unsigned int height;     // Height in pixels
    unsigned int char_width; // Width in characters
    unsigned int char_height; // Height in characters
    unsigned int pitch;     // Framebuffer pitch in pixels
    uint8_t* framebuffer;   // Framebuffer for dithering (8-bit palette indices)
} V4piContext;

// Global variable hosting the default V4P context
V4piContext v4pi_defaultContextSingleton;
V4piContextP v4pi_defaultContext = &v4pi_defaultContextSingleton;

// Variables hosting current context and related properties
V4piContextP v4pi_context = &v4pi_defaultContextSingleton;
V4pCoord v4p_displayWidth;
V4pCoord v4p_displayHeight;

// Function to get the current display for event handling
static void* v4pi_get_current_display() {
    if (v4pi_context && v4pi_context->display) {
        return v4pi_context->display;
    }
    return NULL;
}

/**
 * Convert V4P color to libcaca 16-bit ARGB format
 * V4P uses 256-color palette, we convert to 16-bit ARGB (4 bits per channel)
 */
static uint16_t v4p_color_to_caca_argb16(V4pColor c) {
    // Get RGB components from V4P palette
    int r = V4P_PALETTE_R(c);
    int g = V4P_PALETTE_G(c);
    int b = V4P_PALETTE_B(c);
    
    // Convert to 16-bit ARGB format (0xARGB) with 4 bits per channel
    // Scale 8-bit values to 4-bit: value >> 4
    return 0xF000 | ((r >> 4) << 8) | ((g >> 4) << 4) | (b >> 4);
}

// prepare things before V4P engine scanline loop
Boolean v4pi_start() {
    // Clear the framebuffer if it exists
    if (v4pi_context->framebuffer) {
        memset(v4pi_context->framebuffer, 0, v4pi_context->height * v4pi_context->pitch * sizeof(uint8_t));
    }
    
    return success;
}



// finalize things after V4P engine scanline loop
Boolean v4pi_end() {
    // Dither the framebuffer to the canvas
    if (v4pi_context->framebuffer && v4pi_context->dither && v4pi_context->canvas) {
        caca_dither_bitmap(v4pi_context->canvas, 0, 0,
                          v4pi_context->char_width, v4pi_context->char_height,
                          v4pi_context->dither, v4pi_context->framebuffer);
    }
    
    // Refresh the display to show our changes if we have a display
    if (v4pi_context->display) {
        caca_refresh_display(v4pi_context->display);
    }
    
    return success;
}

// Draw an horizontal video slice with color 'c'
// For libcaca, we write to framebuffer for later dithering
Boolean v4pi_slice(V4pCoord y, V4pCoord x0, V4pCoord x1, V4pColor c) {
    if (!v4pi_context->framebuffer || y >= (V4pCoord)v4pi_context->height || x0 >= (V4pCoord)v4pi_context->width) {
        return success;
    }
    
    // Ensure x1 is within bounds
    if (x1 > (V4pCoord)v4pi_context->width) {
        x1 = (V4pCoord)v4pi_context->width;
    }
    
    // For 8-bit framebuffer, we store the palette index directly
    // This is much more efficient than storing 32-bit ARGB values
    uint8_t* row_start = v4pi_context->framebuffer + y * v4pi_context->pitch;
    memset(row_start + x0, c, x1 - x0);
    
    return success;
}

// Prepare things before the very first graphic rendering
Boolean v4pi_init(int quality, Boolean fullscreen) {
    // Initialize libcaca
    // Create a canvas with reasonable character dimensions
    // libcaca works in characters, not pixels

    caca_display_t* dp = caca_create_display(NULL);
    if (! dp) {
        v4p_error("v4pi_init failed, could not create caca display\n");
        return failure;
    }

    caca_canvas_t* cv = caca_get_canvas(dp);
    
    // Set display title
    caca_set_display_title(dp, "V4P libcaca Backend");

    int char_width = caca_get_canvas_width(cv);
    int char_height = caca_get_canvas_height(cv);

    // For libcaca, we may need to scale pixel coordinates to character coordinates
    int pixel_width = char_width * 2;
    int pixel_height = char_height * 3;

    // Create dither object for 8-bit palette format (much more efficient)
    caca_dither_t* dither = caca_create_dither(8, pixel_width, pixel_height, pixel_width,
                                               0, 0, 0, 0);
    if (!dither) {
        v4p_error("v4pi_init failed, could not create dither\n");
        caca_free_display(dp);
        return failure;
    }

    // Set dither parameters for better quality
    // caca_set_dither_algorithm(dither, "ordered2");
    caca_set_dither_algorithm(dither, "none");
    // caca_set_dither_color(dither, "full16");
    caca_set_dither_charset(dither, "shades");
    // caca_set_dither_antialias(dither, "prefilter");
    caca_set_dither_gamma(dither, 0.5f);

    // Set the dither palette to match V4P's 256-color palette
    // libcaca expects 12-bit values (0-4095), so scale 8-bit V4P values (0-255) to 12-bit
    uint32_t red[256], green[256], blue[256], alpha[256];
    for (int i = 0; i < 256; i++) {
        red[i] = (v4p_palette[i][0] * 4095) / 255;  // Scale 0-255 to 0-4095
        green[i] = (v4p_palette[i][1] * 4095) / 255;
        blue[i] = (v4p_palette[i][2] * 4095) / 255;
        alpha[i] = 4095;  // Fully opaque
    }
    caca_set_dither_palette(dither, red, green, blue, alpha);

    // Allocate framebuffer (8-bit palette indices - much more memory efficient)
    uint8_t* framebuffer = (uint8_t*)malloc(pixel_width * pixel_height * sizeof(uint8_t));
    if (!framebuffer) {
        v4p_error("v4pi_init failed, could not allocate framebuffer\n");
        caca_free_dither(dither);
        caca_free_display(dp);
        return failure;
    }

    // The default context holds the main canvas/display
    v4pi_defaultContextSingleton.canvas = cv;
    v4pi_defaultContextSingleton.display = dp;
    v4pi_defaultContextSingleton.dither = dither;
    v4pi_defaultContextSingleton.width = pixel_width;
    v4pi_defaultContextSingleton.height = pixel_height;
    v4pi_defaultContextSingleton.char_width = char_width;
    v4pi_defaultContextSingleton.char_height = char_height;
    v4pi_defaultContextSingleton.pitch = pixel_width;  // Pitch in pixels
    v4pi_defaultContextSingleton.framebuffer = framebuffer;
    v4pi_setContext(v4pi_defaultContext);
    
    return success;
}

// Create a new buffer-like V4P context
V4piContextP v4pi_newContext(int width, int height) {
    V4piContextP c = (V4piContextP) malloc(sizeof(V4piContext));
    if (!c)
        return NULL;
    
    c->width = width;
    c->height = height;
    c->char_width = width / 2;  // Convert pixels to characters
    c->char_height = height / 2;
    c->pitch = width;  // Pitch in pixels
    
    // Create canvas for offscreen contexts
    c->canvas = caca_create_canvas(c->char_width, c->char_height);
    
    // Create dither object for offscreen contexts (8-bit palette format)
    c->dither = caca_create_dither(8, width, height, width, 0, 0, 0, 0);
    if (c->dither) {
        caca_set_dither_algorithm(c->dither, "fstein");
        caca_set_dither_color(c->dither, "full16");
        caca_set_dither_charset(c->dither, "blocks");
        caca_set_dither_antialias(c->dither, "prefilter");

        // Set the dither palette to match V4P's 256-color palette
        uint32_t red[256], green[256], blue[256], alpha[256];
        for (int i = 0; i < 256; i++) {
            red[i] = (v4p_palette[i][0] * 4095) / 255;
            green[i] = (v4p_palette[i][1] * 4095) / 255;
            blue[i] = (v4p_palette[i][2] * 4095) / 255;
            alpha[i] = 4095;
        }
        caca_set_dither_palette(c->dither, red, green, blue, alpha);
    }
    
    // Allocate framebuffer (8-bit palette indices)
    c->framebuffer = (uint8_t*)malloc(width * height * sizeof(uint8_t));
    if (!c->framebuffer) {
        if (c->dither) caca_free_dither(c->dither);
        if (c->canvas) caca_free_canvas(c->canvas);
        free(c);
        return NULL;
    }
    
    c->display = NULL;  // No display for offscreen contexts
    
    return c;
}

// free a V4P context
void v4pi_destroyContext(V4piContextP c) {
    if (!c || c == v4pi_defaultContext)
        return;
    
    if (c->framebuffer)
        free(c->framebuffer);
    if (c->dither)
        caca_free_dither(c->dither);
    if (c->canvas)
        caca_free_canvas(c->canvas);
    if (c->display)
        caca_free_display(c->display);
    
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
    if (v4pi_defaultContextSingleton.framebuffer)
        free(v4pi_defaultContextSingleton.framebuffer);
    if (v4pi_defaultContextSingleton.dither)
        caca_free_dither(v4pi_defaultContextSingleton.dither);
    if (v4pi_defaultContextSingleton.display)
        caca_free_display(v4pi_defaultContextSingleton.display);
}