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

// UI state variables
static int slider_value = 50;
static float progress_value = 0.5f;
static char text_buffer[64] = "Hello Nuklear!";
static int checkbox_value = 1;

// Custom styling function to make the UI more colorful
void setup_colorful_style(struct nk_context *ctx) {
    struct nk_color table[NK_COLOR_COUNT];
    
    // Set up a colorful theme
    table[NK_COLOR_TEXT] = nk_rgba(240, 240, 240, 255);
    table[NK_COLOR_WINDOW] = nk_rgba(50, 50, 60, 215);
    table[NK_COLOR_HEADER] = nk_rgba(180, 40, 40, 220);
    table[NK_COLOR_BORDER] = nk_rgba(100, 100, 100, 255);
    table[NK_COLOR_BUTTON] = nk_rgba(180, 40, 40, 255);
    table[NK_COLOR_BUTTON_HOVER] = nk_rgba(190, 70, 70, 255);
    table[NK_COLOR_BUTTON_ACTIVE] = nk_rgba(200, 100, 100, 255);
    table[NK_COLOR_TOGGLE] = nk_rgba(50, 50, 60, 255);
    table[NK_COLOR_TOGGLE_HOVER] = nk_rgba(45, 45, 55, 255);
    table[NK_COLOR_TOGGLE_CURSOR] = nk_rgba(180, 40, 40, 255);
    table[NK_COLOR_SELECT] = nk_rgba(180, 40, 40, 255);
    table[NK_COLOR_SELECT_ACTIVE] = nk_rgba(190, 70, 70, 255);
    table[NK_COLOR_SLIDER] = nk_rgba(180, 40, 40, 255);
    table[NK_COLOR_SLIDER_CURSOR] = nk_rgba(190, 70, 70, 255);
    table[NK_COLOR_SLIDER_CURSOR_HOVER] = nk_rgba(200, 100, 100, 255);
    table[NK_COLOR_SLIDER_CURSOR_ACTIVE] = nk_rgba(210, 130, 130, 255);
    table[NK_COLOR_PROPERTY] = nk_rgba(50, 50, 60, 255);
    table[NK_COLOR_EDIT] = nk_rgba(50, 50, 60, 225);
    table[NK_COLOR_EDIT_CURSOR] = nk_rgba(190, 190, 200, 255);
    table[NK_COLOR_COMBO] = nk_rgba(50, 50, 60, 255);
    table[NK_COLOR_CHART] = nk_rgba(50, 50, 60, 255);
    table[NK_COLOR_CHART_COLOR] = nk_rgba(180, 40, 40, 255);
    table[NK_COLOR_CHART_COLOR_HIGHLIGHT] = nk_rgba(255, 0, 0, 255);
    table[NK_COLOR_SCROLLBAR] = nk_rgba(50, 50, 60, 255);
    table[NK_COLOR_SCROLLBAR_CURSOR] = nk_rgba(180, 40, 40, 255);
    table[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = nk_rgba(190, 70, 70, 255);
    table[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = nk_rgba(200, 100, 100, 255);
    table[NK_COLOR_TAB_HEADER] = nk_rgba(180, 40, 40, 220);

    nk_style_from_table(ctx, table);
    
    // Additional custom styling for specific elements
    ctx->style.window.fixed_background = nk_style_item_color(nk_rgba(45, 45, 55, 255));
    ctx->style.window.header.normal = nk_style_item_color(nk_rgba(180, 40, 40, 255));
    ctx->style.window.header.hover = nk_style_item_color(nk_rgba(190, 70, 70, 255));
    ctx->style.window.header.active = nk_style_item_color(nk_rgba(200, 100, 100, 255));
    
    // Button styling
    ctx->style.button.normal = nk_style_item_color(nk_rgba(180, 40, 40, 255));
    ctx->style.button.hover = nk_style_item_color(nk_rgba(190, 70, 70, 255));
    ctx->style.button.active = nk_style_item_color(nk_rgba(200, 100, 100, 255));
    ctx->style.button.border_color = nk_rgba(100, 100, 100, 255);
    ctx->style.button.rounding = 4.0f;
    ctx->style.button.padding = nk_vec2(6, 4);
    
    // Slider styling
    ctx->style.slider.cursor_normal = nk_style_item_color(nk_rgba(190, 70, 70, 255));
    ctx->style.slider.cursor_hover = nk_style_item_color(nk_rgba(200, 100, 100, 255));
    ctx->style.slider.cursor_active = nk_style_item_color(nk_rgba(210, 130, 130, 255));
    ctx->style.slider.bar_normal = nk_rgba(180, 40, 40, 255);
    ctx->style.slider.bar_hover = nk_rgba(190, 70, 70, 255);
    ctx->style.slider.bar_active = nk_rgba(200, 100, 100, 255);
    
    // Text styling
    ctx->style.text.color = nk_rgba(240, 240, 240, 255);
}

Boolean g4p_onInit(int quality, Boolean fullscreen) {
    // Initialize v4p
    v4p_init2(quality, fullscreen);
    
    v4p_setBGColor(V4P_BLACK);
    
    // Initialize Nuklear with v4p backend
    // Use default resolution for now
    nk_ctx = nk_v4p_init(v4p_getScene(), v4p_displayWidth, v4p_displayHeight);

    if (!nk_ctx) {
        v4p_error("Failed to initialize Nuklear");
        return failure;
    }
    
    // Set custom colorful style instead of default
    setup_colorful_style(nk_ctx);
    
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

        // Clear screen
    v4p_clearScene();
    
    // Set up Nuklear UI
    if (nk_begin(nk_ctx, "Nuklear Demo", nk_rect(50, 50, 300, 400),
                NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
                NK_WINDOW_CLOSABLE|NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
    {
        nk_layout_row_dynamic(nk_ctx, 30, 1);
        if (nk_button_label(nk_ctx, "Button")) {
            // Button clicked
            v4p_trace(G4P, "Button clicked!");
        }
    }
    nk_end(nk_ctx);
    
    // Render Nuklear UI
    nk_v4p_render(nk_ctx);

    return success;
}

Boolean g4p_onFrame() {
    
    // Render the v4p scene (which now includes Nuklear UI)
    return v4p_render();
    // return failure; // Return failure to stop after one frame for demo purposes
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
