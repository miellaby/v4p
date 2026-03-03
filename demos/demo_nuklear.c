/*
 * Nuklear IMGUI Demo for v4p
 * Demonstrates integration of Nuklear UI with v4p/g4p
 */

#include "v4p.h"
#include "g4p.h"

// Nuklear configuration
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_IMPLEMENTATION
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_INCLUDE_SOFTWARE_FONT

#include "addons/nuklear/nuklear.h"
#define NK_V4P_IMPLEMENTATION
#include "addons/nuklear/nuklear_v4p.h"

// Nuklear context and state
static struct nk_context *nk_ctx = NULL;
static struct v4p_pl pixel_layout = {0};

// UI state variables
static int slider_value = 50;
static float progress_value = 0.5f;
static char text_buffer[64] = "Hello Nuklear!";
static int checkbox_value = 1;

Boolean g4p_onInit(int quality, Boolean fullscreen) {
    // Initialize v4p
    v4p_init2(quality, fullscreen);
    
    v4p_setBGColor(V4P_BLACK);
    
    // Set up pixel layout for v4p (assuming 32-bit ARGB format)
    pixel_layout.bytesPerPixel = 4;
    pixel_layout.rshift = 16;  // Red at bit 16
    pixel_layout.gshift = 8;   // Green at bit 8
    pixel_layout.bshift = 0;   // Blue at bit 0
    pixel_layout.ashift = 24;  // Alpha at bit 24
    pixel_layout.rloss = 0;
    pixel_layout.gloss = 0;
    pixel_layout.bloss = 0;
    pixel_layout.aloss = 0;
    
    // Initialize Nuklear with v4p backend
    // Use default resolution for now
    nk_ctx = nk_v4p_init(v4p_getScene(), v4p_displayWidth, v4p_displayHeight, pixel_layout);

    if (!nk_ctx) {
        v4p_error("Failed to initialize Nuklear");
        return failure;
    }
    
    // Set Nuklear style
    nk_style_default(nk_ctx);
    
    return success;
}

Boolean g4p_onTick(Int32 deltaTime) {
    // Update progress value for demo
    progress_value += 0.0001f * deltaTime;
    if (progress_value > 1.0f) progress_value = 0.0f;
    
    // Handle Nuklear input - this would be integrated with g4p input system
    // For now, we'll just set up basic input handling
    
    // Nuklear input handling would go here
    // nk_input_begin(nk_ctx);
    // ... process mouse/keyboard events ...
    // nk_input_end(nk_ctx);
    
    return success;
}

Boolean g4p_onFrame() {
    // Clear screen
    v4p_clearScene();
    
    // Set up Nuklear UI
    if (nk_begin(nk_ctx, "Nuklear Demo", nk_rect(50, 50, 300, 400),
                NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
                NK_WINDOW_CLOSABLE|NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
    {
        // Slider
        nk_layout_row_dynamic(nk_ctx, 30, 1);
        nk_label(nk_ctx, "Slider:", NK_TEXT_LEFT);
        nk_slider_int(nk_ctx, 0, &slider_value, 100, 1);
        
        // Progress bar
        nk_layout_row_dynamic(nk_ctx, 30, 1);
        nk_label(nk_ctx, "Progress:", NK_TEXT_LEFT);
        {
            nk_size progress_size = (nk_size)(progress_value * 100);
            nk_progress(nk_ctx, &progress_size, 100, NK_MODIFIABLE);
        }
        
        // Checkbox
        nk_layout_row_dynamic(nk_ctx, 30, 1);
        nk_checkbox_label(nk_ctx, "Checkbox", &checkbox_value);
        
        // Text input
        nk_layout_row_dynamic(nk_ctx, 30, 1);
        nk_label(nk_ctx, "Text:", NK_TEXT_LEFT);
        {
            int len = 64;
            nk_edit_string(nk_ctx, NK_EDIT_FIELD, text_buffer, &len, 64, nk_filter_default);
        }
        
        // Button
        nk_layout_row_dynamic(nk_ctx, 30, 1);
        if (nk_button_label(nk_ctx, "Button")) {
            // Button clicked
            // v4p_trace("G4P", "Button clicked!");
        }
        
        // Property editor
        nk_layout_row_dynamic(nk_ctx, 200, 1);
        nk_property_int(nk_ctx, "Property:", 0, &slider_value, 100, 1, 1);
    }
    nk_end(nk_ctx);
    
    // Render Nuklear UI
    nk_v4p_render(nk_ctx);
    
    // Render the v4p scene (which now includes Nuklear UI)
    return v4p_render();
}

void g4p_onQuit() {
    if (nk_ctx) {
        nk_v4p_shutdown(nk_ctx);
        nk_ctx = NULL;
    }
    v4p_quit();
}

int main(int argc, char** argv) {
    return g4p_main(argc, argv);
}
