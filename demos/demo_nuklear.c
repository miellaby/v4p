/*
 * Nuklear IMGUI Demo for v4p
 * Demonstrates integration of Nuklear UI with v4p/g4p
 */

#include "v4p.h"
#include "game_engine/g4p.h"
#include <string.h>

// Nuklear configuration
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_IMPLEMENTATION
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_INCLUDE_SOFTWARE_FONT

#include "nuklear/nuklear.h"
#define NK_V4P_IMPLEMENTATION
#include "nuklear/nuklear_v4p.h"

// Nuklear context and state
static struct nk_context *nk_ctx = NULL;

// UI state variables
static int slider_value = 50;
static nk_size progress_value = 50;
static char text_buffer[64] = "Hello Nuklear!";
static int checkbox_value = 1;
static int radio_value = 1;
static int combo_selection = 0;
static const char *combo_items[] = {"Item 1", "Item 2", "Item 3", "Item 4"};
static int property_int[3] = {20, 40, 60};
static float property_float[2] = {0.5f, 0.8f};

// Custom styling function to make the UI more colorful
void setup_colorful_style(struct nk_context *ctx) {
    struct nk_color table[NK_COLOR_COUNT];
    
    // Set up a colorful theme
    table[NK_COLOR_TEXT] = nk_rgba(240, 240, 240, 255);
    table[NK_COLOR_WINDOW] = nk_rgba(50, 50, 60, 215);
    table[NK_COLOR_HEADER] = nk_rgba(180, 40, 40, 220);
    table[NK_COLOR_BORDER] = nk_rgba(100, 100, 100, 255);
    table[NK_COLOR_BUTTON] = nk_rgba(170, 30, 30, 255);
    table[NK_COLOR_BUTTON_HOVER] = nk_rgba(200, 70, 70, 255);
    table[NK_COLOR_BUTTON_ACTIVE] = nk_rgba(230, 130, 130, 255);
    table[NK_COLOR_TOGGLE] = nk_rgba(20, 20, 20, 255);
    table[NK_COLOR_TOGGLE_HOVER] = nk_rgba(230, 130, 130, 255);
    table[NK_COLOR_TOGGLE_CURSOR] = nk_rgba(180, 40, 40, 220);
    table[NK_COLOR_SELECT] = nk_rgba(180, 40, 40, 255);
    table[NK_COLOR_SELECT_ACTIVE] = nk_rgba(230, 130, 130, 255);
    table[NK_COLOR_SLIDER] = nk_rgba(140, 40, 40, 255);
    table[NK_COLOR_SLIDER_CURSOR] = nk_rgba(240, 240, 240, 255);
    table[NK_COLOR_SLIDER_CURSOR_HOVER] = nk_rgba(200, 70, 70, 255);
    table[NK_COLOR_SLIDER_CURSOR_ACTIVE] = nk_rgba(230, 130, 130, 255);
    table[NK_COLOR_PROPERTY] = nk_rgba(50, 50, 60, 255);
    table[NK_COLOR_EDIT] = nk_rgba(50, 50, 60, 225);
    table[NK_COLOR_EDIT_CURSOR] = nk_rgba(190, 190, 200, 255);
    table[NK_COLOR_COMBO] = nk_rgba(50, 50, 60, 255);
    table[NK_COLOR_CHART] = nk_rgba(50, 50, 60, 255);
    table[NK_COLOR_CHART_COLOR] = nk_rgba(180, 40, 40, 255);
    table[NK_COLOR_CHART_COLOR_HIGHLIGHT] = nk_rgba(255, 0, 0, 255);
    table[NK_COLOR_SCROLLBAR] = nk_rgba(50, 50, 60, 255);
    table[NK_COLOR_SCROLLBAR_CURSOR] = nk_rgba(170, 30, 30, 255);
    table[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = nk_rgba(200, 70, 70, 255);
    table[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = nk_rgba(230, 130, 130, 255);
    table[NK_COLOR_TAB_HEADER] = nk_rgba(180, 40, 40, 220);
    table[NK_COLOR_KNOB] = nk_rgba(20, 20, 20, 255);
    table[NK_COLOR_KNOB_CURSOR] = nk_rgba(170, 30, 30, 255);
    table[NK_COLOR_KNOB_CURSOR_HOVER] = nk_rgba(200, 100, 100, 255);
    table[NK_COLOR_KNOB_CURSOR_ACTIVE] = nk_rgba(230, 130, 130, 255);
    nk_style_from_table(ctx, table);
}

int g4p_onInit(int quality, bool fullscreen) {
    // Initialize v4p
    v4p_init2(quality, fullscreen);
    // v4p_setView(0, 0, 1.2 * v4p_displayWidth, 1.2 * v4p_displayHeight);
    
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

int g4p_onTick(int32_t deltaTime) {
    static int32_t elapsedTime = 0;
    elapsedTime += deltaTime;

    // Update progress value for demo
    progress_value = (elapsedTime / 1000) % 100;
    
    // Handle Nuklear input using G4P events
    nk_input_begin(nk_ctx);
    
    G4pEvent event;
    while (g4p_pollEvent(&event)) {
        if (event.type == G4P_EVENT_NONE) continue;
        nk_v4p_handle_event(nk_ctx, &event);
    }
    
    nk_input_end(nk_ctx);

    // Clear screen
    v4p_clearScene();
    
    // Set up Nuklear UI
    if (nk_begin(nk_ctx, "Nuklear Demo", nk_rect(120, 10, 400, 460),
                NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
                NK_WINDOW_CLOSABLE|NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
    {
        // Header
        nk_layout_row_dynamic(nk_ctx, 20, 1);
        nk_label(nk_ctx, "Nuklear IMGUI Demo", NK_TEXT_ALIGN_CENTERED);
        nk_label(nk_ctx, "Various UI Elements", NK_TEXT_ALIGN_CENTERED);
        
        // Basic Widgets
        nk_layout_row_dynamic(nk_ctx, 30, 1);
        nk_label(nk_ctx, "Basic Widgets:", NK_TEXT_LEFT);
        
        nk_layout_row_dynamic(nk_ctx, 30, 2);
        if (nk_button_label(nk_ctx, "Button")) {
            v4p_trace(G4P, "Button clicked!");
        }
        
        nk_checkbox_label(nk_ctx, "Checkbox", &checkbox_value);
        
        // Slider
        nk_layout_row_dynamic(nk_ctx, 30, 1);
        nk_label(nk_ctx, "Slider:", NK_TEXT_LEFT);
        nk_layout_row_dynamic(nk_ctx, 30, 1);
        nk_slider_int(nk_ctx, 0, &slider_value, 100, 5);
        nk_label(nk_ctx, "Progress:", NK_TEXT_LEFT);
        nk_progress(nk_ctx, &progress_value, 100, NK_MODIFIABLE);
        
        // Radio buttons
        nk_layout_row_dynamic(nk_ctx, 30, 1);
        nk_label(nk_ctx, "Radio Buttons:", NK_TEXT_LEFT);
        nk_layout_row_dynamic(nk_ctx, 30, 1);
        if (nk_option_label(nk_ctx, "Option 1", radio_value == 1)) radio_value = 1;
        if (nk_option_label(nk_ctx, "Option 2", radio_value == 2)) radio_value = 2;
        if (nk_option_label(nk_ctx, "Option 3", radio_value == 3)) radio_value = 3;
        
        // Combo box
        nk_layout_row_dynamic(nk_ctx, 30, 1);
        nk_label(nk_ctx, "Combo Box:", NK_TEXT_LEFT);
        nk_layout_row_dynamic(nk_ctx, 30, 1);
        combo_selection = nk_combo(nk_ctx, combo_items, NK_LEN(combo_items), combo_selection, 25, nk_vec2(200, 200));
        
        // Property editor
        nk_layout_row_dynamic(nk_ctx, 30, 1);
        nk_label(nk_ctx, "Property Editor:", NK_TEXT_LEFT);
        nk_layout_row_dynamic(nk_ctx, 30, 1);
        nk_property_int(nk_ctx, "Int Property 1:", 0, &property_int[0], 100, 1, 1);
        nk_property_int(nk_ctx, "Int Property 2:", 0, &property_int[1], 100, 1, 1);
        nk_property_float(nk_ctx, "Float Property 1:", 0, &property_float[0], 1.0f, 0.01f, 0.1f);
        
        // Text input
        nk_layout_row_dynamic(nk_ctx, 30, 1);
        nk_label(nk_ctx, "Text Input:", NK_TEXT_LEFT);
        nk_layout_row_dynamic(nk_ctx, 30, 1);
        {
            int len = strlen(text_buffer);
            nk_edit_string(nk_ctx, NK_EDIT_FIELD, text_buffer, &len, 64, nk_filter_default);
        }
        

        
        // Chart
        nk_layout_row_dynamic(nk_ctx, 30, 1);
        nk_label(nk_ctx, "Chart:", NK_TEXT_LEFT);
        nk_layout_row_dynamic(nk_ctx, 100, 1);
        {
            float values[] = {26.0f, 13.0f, 30.0f, 15.0f, 25.0f, 10.0f, 20.0f, 40.0f, 12.0f, 8.0f, 22.0f, 28.0f};
            nk_chart_begin(nk_ctx, NK_CHART_LINES, NK_LEN(values), 0, 50);
            for (int i = 0; i < NK_LEN(values); ++i) {
                nk_chart_push(nk_ctx, values[i]);
            }
            nk_chart_end(nk_ctx);
        }
    }
    nk_end(nk_ctx);
    
    // Render Nuklear UI
    nk_v4p_render(nk_ctx);

    return success;
}

int g4p_onFrame() {
    
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
