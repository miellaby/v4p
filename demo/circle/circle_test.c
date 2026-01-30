#include "game_engine.h"
#include "v4p.h"
#include "v4pi.h"
#include "lowmath.h"  // For iabs() function
#define RADIUS 20
#define SPACING 70
#define GRID_SIZE 30
#define DEPTH 6
V4pPolygonP circle_matrix[GRID_SIZE][GRID_SIZE];

Boolean g4pOnInit() {
    int i, j;
    v4pDisplayInit(1, 0);  // Normal quality, windowed
    v4p_init();
    v4p_setBGColor(V4P_WHITE);  // Black background

    // Create a prototype
    V4pPolygonP original = v4p_newDisk(V4P_ABSOLUTE, V4P_RED, DEPTH, 0, 0, RADIUS);

    // Create a grid of clones
    for (j = 0; j < GRID_SIZE; j++) {
        for (i = 0; i < GRID_SIZE; i++) {
            circle_matrix[j][i] = v4p_addClone(original);
        }
    }

    return success;
}

int elapsedTime = 0;

Boolean g4pOnTick(Int32 deltaTime) {
    int i, j;

    elapsedTime += deltaTime;

    // Apply different zoom levels to each box
    for (j = 0; j < GRID_SIZE; j++) {
        for (i = 0; i < GRID_SIZE; i++) {
            // Calculate zoom factor using triangle wave
            int phase = (i * 3145 + j * 4791 + elapsedTime) % 256;  // 0 to 255
            int scale = 128 + (255 - iabs(phase - 128));

            v4pi_debug("scale %d\n", scale);

            // Transform clones with different zoom levels
            v4p_transform(circle_matrix[j][i], i * SPACING, j * SPACING + 100, 0, 0, scale, scale);
        }
    }

    return success;  // Keep running indefinitely
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