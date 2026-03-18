// Demo: Rotating arcs with varying speed
#include "v4p.h"
#include "game_engine/g4p.h"
#include <math.h>

#define NUM_ARCS 10
// 16
#define CENTER_X 320
#define CENTER_Y 240
#define RADIUS 150

V4pPolygonP arcs[NUM_ARCS];
float angle = 0.0f;
float speed = 0.0001f;
bool increasing = true;

int g4p_onInit(int quality, bool fullscreen) {
    v4p_init2(quality, fullscreen);
    v4p_setBGColor(V4P_BLACK);
    
    // Create arcs
    for (int i = 0; i < NUM_ARCS; i++) {
        V4pPolygonP proto = v4p_new(V4P_ABSOLUTE, (i % 2 == 0) ? V4P_RED : V4P_BLUE, i + 1);
        
        // Initial positions
        double start_angle = (i ) * 2 * M_PI / NUM_ARCS;
        double end_angle = (i + 1) * 2 * M_PI / NUM_ARCS;

        int x1 = CENTER_X + RADIUS * cos(start_angle);
        int y1 = CENTER_Y - RADIUS * sin(start_angle);
        int x2 = CENTER_X + RADIUS * cos(end_angle);
        int y2 = CENTER_Y - RADIUS * sin(end_angle);

        v4p_addPoint(proto, CENTER_X, CENTER_Y);  // From center
        v4p_addPoint(proto, x1, y1);
        v4p_addEllipseCenter(proto, CENTER_X, CENTER_Y, RADIUS, RADIUS);
        v4p_addPoint(proto, x2, y2);
        v4p_addPoint(proto, CENTER_X, CENTER_Y);  // To center
        v4p_setAnchor(proto, CENTER_X, CENTER_Y); // rotation anchor is the center
        arcs[i] = v4p_addClone(proto);
    }
    
    return success;
}

int g4p_onTick(int32_t deltaTime) {
    // Update rotation speed
    if (increasing) {
        speed += 0.001f;
        if (speed > 0.2f) increasing = false;
    } else {
        speed -= 0.001f;
        if (speed < 0.001f) increasing = true;
    }
    
    // Rotate all arcs
    angle += speed;
    for (int i = 0; i < NUM_ARCS; i++) {
        v4p_transform(arcs[i], 0, 0, angle * 2 * 512.0f / 360.0f, 0, 256, 256 + 128);
    }

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
