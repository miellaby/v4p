/**
 * V4P Implementation for DOM (WebAssembly)
 * Minimal implementation using DIV elements for pixels
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>
#include <emscripten.h>
#include <emscripten/html5.h>

#include "v4pi.h"
#include "v4p.h"
#include "v4p_color.h"

// V4P display context variables
V4pCoord v4p_displayWidth = 640;
V4pCoord v4p_displayHeight = 480;

// A display context for DOM backend
typedef struct v4pi_context_s {
    unsigned int width;
    unsigned int height;
    unsigned int pixel_size;  // Cached pixel size in CSS pixels
} V4piContext;

// Global variable hosting the default V4P context
V4piContext v4pi_defaultContextSingleton;
V4piContextP v4pi_defaultContext = &v4pi_defaultContextSingleton;

// Variables hosting current context and related properties
V4piContextP v4pi_context = &v4pi_defaultContextSingleton;

// Initialize the DOM backend
Boolean v4pi_init(int quality, Boolean fullscreen) {
    // Set display dimensions
    v4p_displayWidth = 640;
    v4p_displayHeight = 480;
    
    // Calculate and store pixel size in context
    // Container is 644px wide (640 content + 2px border * 2), so pixel size = 644 / 640 = 1.00625
    v4pi_defaultContext->pixel_size = 644 / v4p_displayWidth;
    if (v4pi_defaultContext->pixel_size <= 0) {
        v4pi_defaultContext->pixel_size = 1; // Fallback to 1px
    }
    
    // Create container div and line structure
    EM_ASM({
        var container = document.getElementById('v4p-container');
        if (!container) {
            container = document.createElement('div');
            container.id = 'v4p-container';
            container.className = 'container';
            container.style.width = $0 + 'px';
            container.style.height = $1 + 'px';
            document.body.appendChild(container);
        }
        
        // Clear existing content
        while (container.firstChild) {
            container.removeChild(container.firstChild);
        }
        
        // Create lines (rows)
        for (var y = 0; y < $1; y++) {
            var line = document.createElement('div');
            line.className = 'line';
            line.id = 'line-' + y;
            container.appendChild(line);
        }
    }, 644, 484);
    
    // Generate CSS classes for the standard V4P palette
    EM_ASM({
        var style = document.createElement('style');
        var css = "";
        
        // The palette data will be passed from C code
        for (var i = 0; i < 256; i++) {
            var r = HEAPU8[$0 + i * 3];
            var g = HEAPU8[$0 + i * 3 + 1];
            var b = HEAPU8[$0 + i * 3 + 2];
            css += '.pixel-color-' + i + ' { background-color: rgb(' + r + ',' + g + ',' + b + '); }\n';
        }
        
        style.textContent = css;
        document.head.appendChild(style);
    }, v4p_palette);
    
    return success;
}

// Start rendering - clear all pixels
Boolean v4pi_start() {
    EM_ASM({
        var container = document.getElementById('v4p-container');
        if (container) {
            // Clear all pixel children efficiently using innerHTML
            var lines = container.children;
            for (var i = 0; i < lines.length; i++) {
                lines[i].innerHTML = '';
            }
        }
    });
    
    return success;
}

// Draw a slice (horizontal line segment)
Boolean v4pi_slice(V4pCoord y, V4pCoord x0, V4pCoord x1, V4pColor c) {
    if (x1 <= x0) {
        return success;  // No slice to draw
    }
    // Add a single elongated pixel that spans the slice width
    // Use the pre-calculated pixel size from context
    int slice_width_px = (x1 - x0) * v4pi_context->pixel_size;
    EM_ASM({
        var line = document.getElementById('line-' + $0);
        if (line) {
            var pixel = document.createElement('div');
            pixel.className = 'pixel pixel-color-' + $2;
            pixel.style.width = $1 + 'px';
            line.appendChild(pixel);
        }
    }, y, slice_width_px, c);
    
    return success;
}

// Finalize rendering
Boolean v4pi_end() {
    // Nothing to do for DOM backend
    return success;
}

// Create a secondary context
V4piContextP v4pi_newContext(int width, int height) {
    // For DOM backend, we don't support multiple contexts yet
    return NULL;
}

// Change the current context
V4piContextP v4pi_setContext(V4piContextP context) {
    // For DOM backend, we don't support multiple contexts yet
    return v4pi_context;
}

// destroy a display context
void v4pi_destroyContext(V4piContextP context) {
    // For DOM backend, we don't support multiple contexts yet
}

// Quit the application
void v4pi_quit() {
    EM_ASM({
        var container = document.getElementById('v4p-container');
        if (container && container.parentNode) {
            container.parentNode.removeChild(container);
        }
    });
}

// clean things before quitting
void v4pi_destroy() {
    // No special cleanup needed for Emscripten
}