#include "g4p.h"
#include "v4p.h"
#include "lowmath.h"

#define BIG_CIRCLE_RADIUS 400
#define SMALL_CIRCLE_RADIUS 1
#define STEPS 512
#define DEPTH 0

V4pPolygonP circles[STEPS];

Boolean g4p_onInit(int quality, Boolean fullscreen) {
    int i;
    v4p_init2(quality, fullscreen);
    v4p_setBGColor(V4P_BLACK);
    // v4p_setView(-v4p_displayWidth * 0.25, -v4p_displayHeight * 0.25, v4p_displayWidth * 1.25, v4p_displayHeight * 1.25);

    // Create a prototype small circle
    V4pPolygonP original = v4p_newDisk(V4P_ABSOLUTE, V4P_WHITE, DEPTH, 0, 0, SMALL_CIRCLE_RADIUS);

    // Create circles at every degree of the big circle
    for (i = 0; i < STEPS; i++) {
        // Create a clone at the calculated position
        circles[i] = v4p_addClone(original);
        v4p_setColor(circles[i], i % 40);
    }

    return success;
}

int elapsedTime = 0;

Boolean g4p_onTick(Int32 deltaTime) {
    elapsedTime += deltaTime;
    int scale = 256;

    // Make the circles pulse with a sine wave effect
    for (int i = 0; i < STEPS; i++) {
        UInt16 angle = ((i * 512) / STEPS) % 512 + 512;  // Convert degree index to angle
        computeCosSin(angle);

        // Calculate position using trigonometry
        // x = center_x + radius * cos(angle)
        // y = center_y + radius * sin(angle)
        int radius = (elapsedTime / 100) % BIG_CIRCLE_RADIUS;
        V4pCoord x = v4p_displayWidth / 2 + ((radius * lwmCosa) >> 8) - radius / 2;
        V4pCoord y = v4p_displayHeight / 2 + ((radius * lwmSina) >> 8);
        v4p_transform(circles[(STEPS + i) % STEPS], x, y, 0, i % 3, scale, scale);  // 256 = 100% scale
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