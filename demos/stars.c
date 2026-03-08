#include "v4p.h"
#include "game_engine/g4p.h"
#include "quick/math.h" 
#include "v4pserial/v4pserial.h"
#include <stdio.h>

#define GRID_SIZE 64
V4pPolygonP stars[GRID_SIZE][GRID_SIZE];
static V4pCoord dimension;

int triangle_wave(int x, int period, int amplitude) {
    return IABS((x % period) - (period / 2)) * amplitude * 2 / period;
}

int g4p_onInit(int quality, Boolean fullscreen) {
    int j, k;
    v4p_init2(quality, fullscreen);
    v4p_setBGColor(V4P_BLACK);

    V4pPolygonP proto = v4p_new(V4P_ABSOLUTE, V4P_RED, 0);
    // patch extracted from: http://upload.wikimedia.org/wikipedia/commons/4/4e/3_stars.svg
    v4p_decodeSVGPath(proto,
                            "M 478.1,5  L 490.5,43.2 L 530.7,43.2 L 498.2,66.8 \
          L 510.6,105 L 478.1,81.4 L 445.6,105 L 458,66.8 \
          L 425.5,43.2 L 465.7,43.2 L 478.1,5 z",
                            4.0f);
    v4p_centerPolygon(proto);

    for (j = 0; j < GRID_SIZE; j++) {
        for (k = 0; k < GRID_SIZE; k++) {
            stars[j][k] = v4p_addClone(proto);
            v4p_setColor(stars[j][k], 1 + (j * k) % 100);
        }
    }
    return success;
}

int g4p_onTick(Int32 deltaTime) {
    static int elapsedTime = 0;
    elapsedTime += deltaTime;

    int unzoom = triangle_wave(elapsedTime, 1024, 512);
    v4p_setView(-v4p_displayWidth * unzoom / 256,
                -v4p_displayHeight * unzoom / 256,
                v4p_displayWidth + v4p_displayWidth * unzoom / 256,
                v4p_displayHeight + v4p_displayHeight * unzoom / 256);

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            v4p_transform(stars[i][j],
                          (dimension * 2) * (2 + i - GRID_SIZE / 2),
                          (dimension * 2) * (2 + i - GRID_SIZE / 2),
                          (i * j) + elapsedTime / 10,
                          (i * j),
                          256,
                          256);
        }
    }
    return success;
}

int g4p_onFrame() {
    v4p_render();
    return success;
}

void g4p_onQuit() {
    v4p_quit();
}

int main(int argc, char** argv) {
    return g4p_main(argc, argv);
}
