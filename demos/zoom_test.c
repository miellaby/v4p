#include "g4p.h"
#include "v4p.h"

#include "lowmath.h"  // For iabs() function

#define BOX_SIZE 50
#define SPACING 70
#define GRID_SIZE 3

V4pPolygonP box_matrix[GRID_SIZE][GRID_SIZE];

Boolean g4p_onInit() {
    int i, j;
    v4p_init2(V4P_QUALITY_NORMAL, V4P_UX_NORMAL);  // Normal quality, windowed
    v4p_setBGColor(V4P_BLACK);  // Black background

    // Create a base box
    V4pPolygonP original_box = v4p_new(V4P_ABSOLUTE, V4P_RED, 5);
    v4p_rect(original_box, 0, 0, BOX_SIZE, BOX_SIZE);
    v4p_setAnchorToCenter(original_box);

    // Create a grid of clones
    for (j = 0; j < GRID_SIZE; j++) {
        for (i = 0; i < GRID_SIZE; i++) {
            box_matrix[j][i] = v4p_addClone(original_box);
        }
    }

    return success;  // Keep running indefinitely
}

int elapsedTime = 0;

Boolean g4p_onTick(Int32 deltaTime) {
    int i, j;

    elapsedTime += deltaTime;

    // Apply different zoom levels to each box
    for (j = 0; j < GRID_SIZE; j++) {
        for (i = 0; i < GRID_SIZE; i++) {
            // Calculate zoom factor using triangle wave
            int phase = (i + j + elapsedTime / 2) % 256;  // 0 to 255
            int zoom_factor
                = 128 + (255 - iabs(phase - 128));  // Creates triangle wave: 128->0->128

            // Transform clones with different zoom levels
            v4p_transform(box_matrix[j][i],
                          i * SPACING,
                          j * SPACING + 100,
                          (i * j * 2) + elapsedTime / 2,
                          0,
                          zoom_factor,
                          zoom_factor);
        }
    }

    return success;  // Keep running indefinitely
}

Boolean g4p_onFrame() {
    return v4p_render();
}

void g4p_onQuit() {
    v4p_quit();
}

int main(int argc, char** argv) {
    return g4p_main(argc, argv);
}