#include <stdio.h>
#include "game_engine.h"
#include "v4p.h"
#include "v4pi.h"
#include "qfont.h"
#include "lowmath.h"

#define STRESS_AMOUNT 10
PolygonP pCol;
PolygonP pColMatrix[STRESS_AMOUNT * 2][STRESS_AMOUNT];

int iu = 0;
int diu = STRESS_AMOUNT;
int liu = 3;

Boolean g4pOnInit() {
    int j, k;

    v4pDisplayInit(1, 0);
    v4p_init();

    v4p_setBGColor(blue);

    pCol = v4p_new(absolute, red, 10);
    qfontDefinePolygonFromString("PORTEZ CE VIEUX WHISKY AU JUGE BLOND QUI FUME",
                                 pCol,
                                 -v4pDisplayWidth / 2,
                                 -v4pDisplayWidth / 32,
                                 v4pDisplayWidth / 16,
                                 v4pDisplayWidth / 16,
                                 12);
    qfontDefinePolygonFromString("THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG",
                                 pCol,
                                 -v4pDisplayWidth / 2,
                                 v4pDisplayWidth / 32 + 12,
                                 v4pDisplayWidth / 16,
                                 v4pDisplayWidth / 16,
                                 12);

    for (j = 0; j < STRESS_AMOUNT * 2; j++) {
        for (k = 0; k < STRESS_AMOUNT; k++) {
            pColMatrix[j][k] = v4p_addClone(pCol);
            v4p_transformClone(pCol,
                               pColMatrix[j][k],
                               v4pDisplayWidth * (2 + 2 * k - STRESS_AMOUNT) * 2,
                               v4pDisplayHeight * (1 + j - STRESS_AMOUNT / 2) / 3,
                               0,
                               10,
                               256,
                               256);
        }
    }
    return success;
}

int elapsedTime = 0;

Boolean g4pOnTick(Int32 deltaTime) {
    elapsedTime += deltaTime;
    int scale = (129 - iabs(elapsedTime % 256 - 128) + (3 * elapsedTime / 2) % 64000) / 64;
    v4p_setView(-v4pDisplayWidth * scale / 256,
                -v4pDisplayHeight * scale / 256,
                v4pDisplayWidth + v4pDisplayWidth * scale / 256,
                v4pDisplayHeight + v4pDisplayHeight * scale / 256);
    return success;
}

Boolean g4pOnFrame() {
    v4p_render();
    return success;
}

void g4pOnQuit() {
    printf("average %d\n", g4pAvgFramePeriod);
    v4pDisplayQuit();
}

int main(int argc, char** argv) {
    return g4pMain(argc, argv);
}