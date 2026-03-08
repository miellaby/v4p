#include "v4p.h"
#include "game_engine/g4p.h"
#include "qfont/qfont.h"
#include "quick/imath.h"
#include <stdio.h>

#define GRID_SIZE 9
V4pPolygonP proto;
V4pPolygonP textMatrix[GRID_SIZE * 2][GRID_SIZE];

int iu = 0;
int diu = GRID_SIZE;
int liu = 3;

int g4p_onInit(int quality, Boolean fullscreen) {
    int j, k;

    v4p_init2(quality, fullscreen);

    v4p_setBGColor(V4P_BLUE);

    proto = v4p_new(V4P_ABSOLUTE, V4P_RED, 10);
    qfontDefinePolygonFromString("PORTEZ CE VIEUX WHISKY AU JUGE BLOND QUI FUME",
                                 proto,
                                 -v4p_displayWidth / 2,
                                 -v4p_displayHeight / 32,
                                 v4p_displayWidth / 16,
                                 v4p_displayHeight / 16,
                                 12);
    qfontDefinePolygonFromString("the quick brown fox jumps over the lazy dog",
                                 proto,
                                 -v4p_displayWidth / 2,
                                 v4p_displayHeight / 32 + 12,
                                 v4p_displayWidth / 16,
                                 v4p_displayHeight / 16,
                                 12);
    qfontDefinePolygonFromInt(1234567890,
                              proto,
                              -v4p_displayWidth / 2,
                              v4p_displayHeight / 32 + v4p_displayHeight / 16 + 24,
                              v4p_displayWidth / 16,
                              v4p_displayHeight / 16,
                              12);
        // all punctation ascii chars from 32 to 47
    qfontDefinePolygonFromString("!\"#$%&'()*+,-./",
                                 proto,
                                 -v4p_displayWidth / 2,
                                 v4p_displayHeight / 32 + 2 * (v4p_displayHeight / 16) + 36,
                                 v4p_displayWidth / 16,
                                 v4p_displayHeight / 16,
                                 12);
    v4p_setAnchorToCenter(proto);
    for (j = 0; j < GRID_SIZE * 2; j++) {
        for (k = 0; k < GRID_SIZE; k++) {
            textMatrix[j][k] = v4p_addClone(proto);
            v4p_transformClone(proto,
                               textMatrix[j][k],
                               v4p_displayWidth * (2.2 + 2 * k - GRID_SIZE) * 2,
                               v4p_displayHeight * (1 + j - GRID_SIZE / 2) / 2,
                               0,
                               10,
                               256,
                               256);
        }
    }
    return success;
}

int g4p_onTick(Int32 deltaTime) {
    static int elapsedTime = 0;
    elapsedTime += deltaTime;

    int scale = (129 - iabs(elapsedTime % 256 - 128) + (3 * elapsedTime / 2) % 64000) / 64;
    v4p_setView(-v4p_displayWidth * scale / 256,
                -v4p_displayHeight * scale / 256,
                v4p_displayWidth + v4p_displayWidth * scale / 256,
                v4p_displayHeight + v4p_displayHeight * scale / 256);

    for (int j = 0; j < GRID_SIZE * 2; j++) {
        for (int k = 0; k < GRID_SIZE; k++) {
            v4p_transform(textMatrix[j][k],
                        v4p_displayWidth * (2.2 + 2 * k - GRID_SIZE) * 2
                        + iabs((elapsedTime / 16) % 256 - 128) * 32,
                        v4p_displayHeight * (1 + j - GRID_SIZE / 2) / 2,
                        // sci-saw angle move
                        (iabs(elapsedTime % 256 - 128) - 128) / 128.0 * 4,
                        10,
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
    printf("average %d\n", g4p_avgFramePeriod);
    v4p_quit();
}

int main(int argc, char** argv) {
    return g4p_main(argc, argv);
}