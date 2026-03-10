// Demo: Draw a set of diameters of a circle using stroke lines
#include "v4p.h"
#include "game_engine/g4p.h"
#include "quick/imath.h"
#include <math.h>

#define NUM_LINES 24
#define CENTER_X 320
#define CENTER_Y 240
#define RADIUS 200

V4pPolygonP lines[NUM_LINES];

int g4p_onInit(int quality, bool fullscreen) {
    v4p_init2(quality, fullscreen);
    
    v4p_setRound(v4p_addCutCorners(v4p_addNew(V4P_ABSOLUTE, V4P_BLUE, 1), 100, 100, v4p_displayWidth - 100, v4p_displayHeight - 100, 40), 1);

    // Create stroke lines forming diameters of a circle
    for (int i = 0; i < NUM_LINES; i++) {
        float angle = (M_PI * i) / (NUM_LINES);
        int x1 = CENTER_X + RADIUS * cos(angle);
        int y1 = CENTER_Y + RADIUS * sin(angle);
        int x2 = CENTER_X - RADIUS * cos(angle);
        int y2 = CENTER_Y - RADIUS * sin(angle);
        v4p_debug("%dx%d-%dx%d\n", x1, y1, x2, y2);

        lines[i] = v4p_addNew(V4P_ABSOLUTE, V4P_RED, 10 + i);
        v4p_setStroke(lines[i], 1);
        v4p_addJump(lines[i]);
        v4p_addPoint(lines[i], x1, y1);
        v4p_addPoint(lines[i], x2, y2);
    }

    return success;
}

int g4p_onTick(int32_t deltaTime) {
    return success;
}

int g4p_onFrame() {
    return v4p_render();
}

void g4p_onQuit() {
    v4p_quit();
}

int main(int argc, char** argv) {
    return g4p_main(argc, argv);
}