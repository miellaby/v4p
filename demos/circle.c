#include "g4p.h"
#include "v4p.h"
#include "quick/math.h"  // For iabs() function
#define DEPTH 6
#define GRID_SIZE 10
V4pPolygonP circle_matrix[GRID_SIZE][GRID_SIZE];

int g4p_onInit(int quality, Boolean fullscreen) {
    int i, j;
    v4p_init2(quality, fullscreen);  // Normal quality, windowed
    v4p_setBGColor(V4P_WHITE);  // Black background
    // Calculate dynamic sizing based on display dimensions
    int min_dimension = IMIN(v4p_displayWidth, v4p_displayHeight);
    int spacing = min_dimension / GRID_SIZE;    // Scale spacing with screen size
    int radius = min_dimension / GRID_SIZE / 3; // Scale radius with screen size
    int grid_width = (GRID_SIZE - 1) * spacing;
    int grid_height = (GRID_SIZE - 1) * spacing;
    v4p_setView(-grid_width / 2, -grid_height / 2, grid_width / 2, grid_height / 2);  // Center the grid on screen
    // Create a prototype with dynamic radius
    V4pPolygonP original = v4p_newDisk(V4P_ABSOLUTE, V4P_RED, DEPTH, 0, 0, radius);

    // Create a grid of clones
    for (j = 0; j < GRID_SIZE; j++) {
        for (i = 0; i < GRID_SIZE; i++) {
            circle_matrix[j][i] = v4p_addClone(original);
        }
    }

    return success;
}

int elapsedTime = 0;

int g4p_onTick(Int32 deltaTime) {
    int i, j;

    elapsedTime += deltaTime;

    // Calculate dynamic spacing (same as in init)
    int min_dimension = IMIN(v4p_displayWidth, v4p_displayHeight);
    int spacing = min_dimension / GRID_SIZE;  // Scale spacing with screen size
    int radius = min_dimension / GRID_SIZE / 3;  // Scale radius with screen size

    // Apply different zoom levels to each box
    for (j = 0; j < GRID_SIZE; j++) {
        for (i = 0; i < GRID_SIZE; i++) {
            // Calculate zoom factor using triangle wave
            int phase = (i * 3145 + j * 4791 + elapsedTime) % 256;  // 0 to 255
            int scale = 128 + (255 - iabs(phase - 128));

            // v4pi_debug("scale %d\n", scale);

            // Transform clones with different zoom levels, centered on screen
            int grid_width = (GRID_SIZE - 1) * spacing;
            int grid_height = (GRID_SIZE - 1) * spacing;
            int center_x = i * spacing - grid_width / 2;
            int center_y = j * spacing - grid_height / 2;
            v4p_transform(circle_matrix[j][i], center_x, center_y, 0, 0, scale, scale);
        }
    }

    return success;  // Keep running indefinitely
}

int g4p_onFrame() {
    v4p_render();
    return success;
}

void g4p_onQuit() {
    // Cleanup if needed
}

int main(int argc, char** argv) {
    return g4p_main(argc, argv);
}