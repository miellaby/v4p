#include "g4p.h"
#include "v4p.h"

#define STRESS_AMOUNT 12
V4pPolygonP proto;
V4pPolygonP squaresMatrix[STRESS_AMOUNT][STRESS_AMOUNT];

int iu = 0;
int diu = STRESS_AMOUNT;
int liu = 3;

Boolean g4p_onInit() {
    int j, k;

    v4p_init2(V4P_QUALITY_NORMAL, V4P_UX_NORMAL);
    v4p_setBGColor(V4P_BLUE);

    proto = v4p_new(V4P_ABSOLUTE, V4P_RED, 10);
    v4p_rect(proto,
             -v4p_displayWidth / 3 + v4p_displayWidth,
             -v4p_displayHeight / 3,
             v4p_displayWidth / 3,
             v4p_displayHeight / 3);

    for (j = 0; j < STRESS_AMOUNT; j++) {
        for (k = 0; k < STRESS_AMOUNT; k++) {
            squaresMatrix[j][k] = v4p_addClone(proto);
            // v4p_transformClone (proto, squaresMatrix[j][k], v4p_displayWidth * (k
            // - STRESS_AMOUNT / 2), v4p_displayWidth * (j - STRESS_AMOUNT / 2),
            // 10, 0);
        }
    }
    return success;
}

Boolean g4p_onTick(Int32 deltaTime) {
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
    v4p_setView(-v4p_displayWidth * i / 256,
                -v4p_displayHeight * i / 256,
                v4p_displayWidth + v4p_displayWidth * i / 256,
                v4p_displayHeight + v4p_displayHeight * i / 256);

    if (liu & 1)
        for (j = 0; j < STRESS_AMOUNT; j++) {
            for (k = 0; k < STRESS_AMOUNT; k++) {
                v4p_transformClone(proto,
                                   squaresMatrix[j][k],
                                   v4p_displayWidth * (1 + k - STRESS_AMOUNT / 2) / 2,
                                   v4p_displayWidth * (1 + j - STRESS_AMOUNT / 2) / 2,
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

Boolean g4p_onFrame() {
    v4p_render();
    return success;
}

void g4p_onQuit() {
    v4p_quit();
}

int main(int argc, char** argv) {
    return g4p_main(argc, argv);
}
