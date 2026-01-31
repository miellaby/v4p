#include "game_engine.h"
#include "v4p.h"
#include "v4pi.h"

#define COLORS_PER_ROW 16
#define COLOR_BOX_SIZE 20
#define SPACING 3

V4pPolygonP box;
V4pPolygonP color_boxes[256];
int current_color = 0;

Boolean g4p_onInit() {
    int i;
    V4pCoord x, y;

    v4pDisplayInit(1, 0);  // Normal quality, windowed
    v4p_init();
    v4p_setBGColor(V4P_BLACK);  // Black background
    box = v4p_new(V4P_ABSOLUTE, i, 4);
    v4p_rect(box, 0, 0, COLOR_BOX_SIZE, COLOR_BOX_SIZE);

    // Create a polygon for each color in the palette
    for (i = 0; i < 256; i++) {
        x = (i % COLORS_PER_ROW) * (COLOR_BOX_SIZE + SPACING);
        y = (i / COLORS_PER_ROW) * (COLOR_BOX_SIZE + SPACING);

        color_boxes[i] = v4p_addClone(box);
        v4p_transformClone(box, color_boxes[i], x, y, 0, 0, 256, 256);
        v4p_setColor(color_boxes[i], i);
    }

    return success;
}

Boolean g4p_onTick(Int32 deltaTime) {
    int i;

    // Check for mouse interaction - select color based on mouse position
    if (g4p_state.buttons[0]) {  // Mouse button pressed
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

    // Add some informative text (if text rendering is available)
    // Note: This would require text rendering functionality which may not be
    // available in the basic v4p engine. You could add this later if needed.

    return success;  // Keep running indefinitely
}

Boolean g4p_onFrame() {
    v4p_render();
    return success;
}

void g4p_onQuit() {
    // // Cleanup
    // int i;
    // for (i = 0; i < 256; i++) {
    //   if (color_boxes[i]) {
    //     v4p_destroy (color_boxes[i]);
    //   }
    // }
    v4pi_quit();
}

int main(int argc, char** argv) {
    return g4p_main(argc, argv);
}
