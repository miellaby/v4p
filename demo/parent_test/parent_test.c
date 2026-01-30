#include "game_engine.h"
#include "v4p.h"
#include "v4pi.h"

#define BOX_SIZE 20
#define SPACING 30
#define GRID_SIZE 4

V4pPolygonP original_box;
V4pPolygonP box_matrix[GRID_SIZE][GRID_SIZE];

Boolean g4pOnInit() {
    int j, k;
    V4pCoord x, y;

    v4pDisplayInit(1, 0);  // Normal quality, windowed
    v4p_init();
    v4p_setBGColor(V4P_BLACK);  // Black background

    // Create original box
    original_box = v4p_new(V4P_ABSOLUTE, V4P_RED, 5);
    v4p_rect(original_box, 0, 0, BOX_SIZE, BOX_SIZE);
    v4p_setAnchorToCenter(original_box);  // Set anchor to center for rotation

    // Create grid of clones using parent-aware methods (now default)
    for (j = 0; j < GRID_SIZE; j++) {
        for (k = 0; k < GRID_SIZE; k++) {
            x = k * SPACING;
            y = j * SPACING;

            // Use standard cloning method (now sets parent automatically)
            box_matrix[j][k] = v4p_addClone(original_box);

            // Set different colors for visualization
            v4p_setColor(box_matrix[j][k], (j * GRID_SIZE + k) * 16);
        }
    }

    return success;
}

int frame_count = 0;

Boolean g4pOnTick(Int32 deltaTime) {
    int i = frame_count++;
    int j, k;

    // Animate using new transform method
    for (j = 0; j < GRID_SIZE; j++) {
        for (k = 0; k < GRID_SIZE; k++) {
            // Transform clones using parent-aware method
            v4p_transform(box_matrix[j][k],
                          k * SPACING,
                          j * SPACING + (i / 4) % 20,
                          (j * k * 2) + (i / 8),
                          0,
                          256,
                          256);
        }
    }

    return success;
}

Boolean g4pOnFrame() {
    v4p_render();
    return success;
}

void g4pOnQuit() {
    // Cleanup if needed
}

int main(int argc, char** argv) {
    return g4pMain(argc, argv);
}