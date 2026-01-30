#include "game_engine.h"
#include "v4p.h"
#include "v4pi.h"

#define STRESS_AMOUNT 12
V4pPolygonP pCol;
V4pPolygonP pColMatrix[STRESS_AMOUNT][STRESS_AMOUNT];

int iu = 0;
int diu = STRESS_AMOUNT;
int liu = 3;

Boolean g4pOnInit() {
    int j, k;

    v4pDisplayInit(1, 0);
    v4p_init();
    v4p_setBGColor(V4P_BLUE);

    pCol = v4p_new(V4P_ABSOLUTE, V4P_RED, 10);
    v4p_rect(pCol,
             -v4pDisplayWidth / 3 + v4pDisplayWidth,
             -v4pDisplayHeight / 3,
             v4pDisplayWidth / 3,
             v4pDisplayHeight / 3);

    for (j = 0; j < STRESS_AMOUNT; j++) {
        for (k = 0; k < STRESS_AMOUNT; k++) {
            pColMatrix[j][k] = v4p_addClone(pCol);
            // v4p_transformClone (pCol, pColMatrix[j][k], v4pDisplayWidth * (k
            // - STRESS_AMOUNT / 2), v4pDisplayWidth * (j - STRESS_AMOUNT / 2),
            // 10, 0);
        }
    }
    return success;
}

Boolean g4pOnTick(Int32 deltaTime) {
    int i = iu, j, k;

    // Use deltaTime for time-based animation instead of frame-based
    // Convert deltaTime from milliseconds to a reasonable animation speed
    float timeFactor = deltaTime / 16.0f;  // Normalize to ~60fps equivalent

    if (diu > 0 && i > 128 * STRESS_AMOUNT)
        diu = -diu;
    if (diu < 0 && i + diu < -100) {
        diu = -diu;
        liu--;
    }
    v4p_setView(-v4pDisplayWidth * i / 256,
                -v4pDisplayHeight * i / 256,
                v4pDisplayWidth + v4pDisplayWidth * i / 256,
                v4pDisplayHeight + v4pDisplayHeight * i / 256);

    if (liu & 1)
        for (j = 0; j < STRESS_AMOUNT; j++) {
            for (k = 0; k < STRESS_AMOUNT; k++) {
                v4p_transformClone(pCol,
                                   pColMatrix[j][k],
                                   v4pDisplayWidth * (1 + k - STRESS_AMOUNT / 2) / 2,
                                   v4pDisplayWidth * (1 + j - STRESS_AMOUNT / 2) / 2,
                                   (j * k) + i / 16,
                                   0,
                                   256,
                                   256);
            }
        }

    // Use time-based movement instead of frame-based
    iu += diu * timeFactor;
    return (liu < 0);
}

Boolean g4pOnFrame() {
    v4p_render();
    return success;
}

void g4pOnQuit() {
    v4pDisplayQuit();
}

int main(int argc, char** argv) {
    return g4pMain(argc, argv);
}
