#include "g4p.h"
#include "v4p.h"


#define BOX_SIZE 20
#define SPACING 30
#define GRID_SIZE 4

V4pPolygonP proto;
V4pPolygonP boxMatrix[GRID_SIZE][GRID_SIZE];

Boolean g4p_onInit() {
    int j, k;

    v4p_init2(V4P_QUALITY_NORMAL, V4P_UX_NORMAL);  // Normal quality, windowed
    v4p_setBGColor(V4P_BLACK);  // Black background

    // Create original box
    proto = v4p_new(V4P_ABSOLUTE, V4P_RED, 5);
    v4p_rect(proto, 0, 0, BOX_SIZE, BOX_SIZE);
    v4p_setAnchorToCenter(proto);  // Set anchor to center for rotation

    // Create grid of clones using parent-aware methods (now default)
    for (j = 0; j < GRID_SIZE; j++) {
        for (k = 0; k < GRID_SIZE; k++) {
            // Clone the prototype box
            boxMatrix[j][k] = v4p_addClone(proto);

            // Set different colors for visualization
            v4p_setColor(boxMatrix[j][k], (j * GRID_SIZE + k) * 16);
        }
    }

    return success;
}

int frame_count = 0;

Boolean g4p_onTick(Int32 deltaTime) {
    int i = frame_count++;
    int j, k;

    // Animate using new transform method
    for (j = 0; j < GRID_SIZE; j++) {
        for (k = 0; k < GRID_SIZE; k++) {
            // Transform clones (parent-based transform)
            v4p_transform(boxMatrix[j][k],
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

Boolean g4p_onFrame() {
    v4p_render();
    return success;
}

void g4p_onQuit() {
    // Cleanup if needed
}

int main(int argc, char** argv) {
    return g4p_main(argc, argv);
}