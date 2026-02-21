#include "g4p.h"
#include "v4p.h"
#include <stdio.h>
#include "addons/qfont/qfont.h"

#define COLORS_PER_ROW 16
#define COLOR_BOX_SIZE 20
#define SPACING 3
#define INFO_Y_POSITION 380 // Position for info text

V4pPolygonP box;
V4pPolygonP color_boxes[256];
V4pPolygonP info_text;  // Polygon for displaying color info
int current_color = 0;

// Function to get color name from v4p_color.h constants
const char* get_color_name(int color_index) {
    switch (color_index) {
        // Basic colors
        case V4P_WHITE: return "WHITE";
        case V4P_SILVER: return "SILVER";
        case V4P_GRAY: return "GRAY";
        case V4P_DARK: return "DARK";
        case V4P_BLACK: return "BLACK";
        
        // Red family
        case V4P_RED: return "RED";
        case V4P_DARK_RED: return "DARK_RED";
        case V4P_ORANGE: return "ORANGE";
        case V4P_PINK: return "PINK";
        case V4P_MAROON: return "MAROON";
        
        // Yellow/Orange family
        case V4P_YELLOW: return "YELLOW";
        case V4P_BROWN: return "BROWN";
        
        // Green family
        case V4P_GREEN: return "GREEN";
        case V4P_DARK_GREEN: return "DARK_GREEN";
        case V4P_OLIVE: return "OLIVE";
        
        // Blue family
        case V4P_BLUE: return "BLUE";
        case V4P_LIGHT_BLUE: return "LIGHT_BLUE";
        case V4P_CYAN: return "CYAN";
        case V4P_TEAL: return "TEAL";
        case V4P_NAVY: return "NAVY";
        
        // Purple family
        case V4P_PURPLE: return "PURPLE";
        
        // Special
        case V4P_LIMEGREEN: return "LIMEGREEN";
        
        default: return "";
    }
}

Boolean g4p_onInit(int quality, Boolean fullscreen) {
    int i;
    V4pCoord x, y;

    v4p_init2(quality, fullscreen);  // Normal quality, windowed
    v4p_setBGColor(V4P_BLACK);  // Black background
    box = v4p_new(V4P_ABSOLUTE, 0, 4);
    v4p_rect(box, 0, 0, COLOR_BOX_SIZE, COLOR_BOX_SIZE);

    // Create a polygon for each color in the palette
    for (i = 0; i < 256; i++) {
        x = (i % COLORS_PER_ROW) * (COLOR_BOX_SIZE + SPACING);
        y = (i / COLORS_PER_ROW) * (COLOR_BOX_SIZE + SPACING);

        color_boxes[i] = v4p_addClone(box);
        v4p_transformClone(box, color_boxes[i], x, y, 0, 0, 256, 256);
        v4p_setColor(color_boxes[i], i);
    }

    // Create info text polygon for displaying color information
    info_text = v4p_addNew(V4P_RELATIVE, V4P_WHITE, 15);
    
    return success;
}

Boolean g4p_onTick(Int32 deltaTime) {
    int i;
    char info_buffer[64];

    // Check for mouse interaction - select color based on mouse position
    if (g4p_state.buttons[G4P_PEN]) {  // Mouse button pressed
        int mouse_x = g4p_state.xpen;
        int mouse_y = g4p_state.ypen;

        // Calculate which color box the mouse is over
        int col = mouse_x / (COLOR_BOX_SIZE + SPACING);
        int row = mouse_y / (COLOR_BOX_SIZE + SPACING);
        int selected_color = row * COLORS_PER_ROW + col;

        // Validate the selection
        if (col >= 0 && col < COLORS_PER_ROW && row >= 0 && selected_color < 256) {
            current_color = selected_color;
        }

        // Adjust polygons
        V4pCoord x, y;
        for (i = 0; i < 256; i++) {
            x = (i % COLORS_PER_ROW) * (COLOR_BOX_SIZE + SPACING);
            y = (i / COLORS_PER_ROW) * (COLOR_BOX_SIZE + SPACING);
            v4p_transformClone(box,
                               color_boxes[i],
                               x,
                               y,
                               (i == current_color ? 5 : 0),
                               0,
                               256,
                               256);
        }
    }

    // Update info text with current color information
    const char* color_name = get_color_name(current_color);
    int r = v4p_palette[current_color][0];
    int g = v4p_palette[current_color][1];
    int b = v4p_palette[current_color][2];
    
    if (color_name[0] != '\0') {
        snprintf(info_buffer, sizeof(info_buffer), "INDEX: %d RGB: <%d,%d,%d> %s", 
                 current_color, r, g, b, color_name);
    } else {
        snprintf(info_buffer, sizeof(info_buffer), "INDEX: %d RGB: <%d,%d,%d>", 
                 current_color, r, g, b);
    }
    
    // Clear and redraw the info text
    v4p_destroyFromScene(info_text);
    info_text = v4p_addNew(V4P_RELATIVE, V4P_WHITE, 15);
    qfontDefinePolygonFromString(info_buffer, info_text, 10, INFO_Y_POSITION, 12, 12, 2);

    return success;  // Keep running indefinitely
}

Boolean g4p_onFrame() {
    v4p_render();
    return success;
}

void g4p_onQuit() {
    v4p_quit();
}

int main(int argc, char** argv) {
    return g4p_main(argc, argv);
}
