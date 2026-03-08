#include "v4p.h"
#include "game_engine/g4p.h"
#include "quick/imath.h"

#define GRID_SIZE 12
V4pPolygonP squares[GRID_SIZE][GRID_SIZE];
static V4pCoord dimension;

int g4p_onInit(int quality, Boolean fullscreen) {
    v4p_init2(quality, fullscreen);
    v4p_setBGColor(V4P_BLUE);
    dimension = IMIN(v4p_displayWidth / 8, v4p_displayHeight / 8);

    V4pPolygonP proto = v4p_new(V4P_ABSOLUTE, V4P_RED, 10);
    v4p_addCorners(proto, -dimension, -dimension, dimension, dimension);

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            squares[i][j] = v4p_addClone(proto);
        }
    }
    return success;
}

int triangle_wave(int x, int period, int amplitude) {
    return IABS((x % period) - (period / 2)) * amplitude * 2 / period;
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
            v4p_transform(squares[i][j],
                          (dimension * 3) * (2 + j - GRID_SIZE / 2),
                          (dimension * 3) * (2 + i - GRID_SIZE / 2),
                          (i * j) + elapsedTime / 10,
                          0,
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
