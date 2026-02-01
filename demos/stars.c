#include <stdio.h>
#include "g4p.h"
#include "v4p.h"
#include "v4pi.h"
#include "v4pserial.h"

#define STRESS_AMOUNT 128
V4pPolygonP proto;
V4pPolygonP stars[STRESS_AMOUNT][STRESS_AMOUNT];

int iu = 0;
int riu = 0;
int diu = STRESS_AMOUNT / 1;
int liu = 3;

Boolean g4p_onInit() {
    int j, k;
    v4pi_init(V4P_QUALITY_NORMAL, V4P_UX_NORMAL);
    v4p_init();
    v4p_setBGColor(V4P_BLACK);

    proto = v4p_new(V4P_ABSOLUTE, V4P_RED, 0);
    // patch extracted from: http://upload.wikimedia.org/wikipedia/commons/4/4e/3_stars.svg
    v4p_decodeSVGPath(proto,
                            "M 478.1,5  L 490.5,43.2 L 530.7,43.2 L 498.2,66.8 \
          L 510.6,105 L 478.1,81.4 L 445.6,105 L 458,66.8 \
          L 425.5,43.2 L 465.7,43.2 L 478.1,5 z",
                            1200);
    v4p_setAnchorToCenter(proto);

    for (j = 0; j < STRESS_AMOUNT; j++) {
        for (k = 0; k < STRESS_AMOUNT; k++) {
            stars[j][k] = v4p_addClone(proto);
            v4p_transformClone(proto,
                               stars[j][k],
                               v4p_displayWidth * (1 + k - STRESS_AMOUNT / 2) / 2,
                               v4p_displayWidth * (1 + j - STRESS_AMOUNT / 2),
                               (j * k) / 2 + riu,
                               1 + k % 12,
                               256,
                               256);
            v4p_setColor(stars[j][k], 1 + (j * k) % 100);
        }
    }
    return success;
}

Boolean g4p_onTick(Int32 deltaTime) {
    int i = iu, j, k;

    // Use deltaTime for time-based animation
    float timeFactor = deltaTime / 16.0f;  // Normalize to ~60fps equivalent

    if (diu > 0 && i > 100 * STRESS_AMOUNT)
        diu = -diu;
    if (diu < 0 && i + diu < -100) {
        diu = -diu;
        liu--;
    }
    v4p_setView(-v4p_displayWidth * i / 256,
                -v4p_displayHeight * i / 256,
                v4p_displayWidth + v4p_displayWidth * i / 256,
                v4p_displayHeight + v4p_displayHeight * i / 256);

    if (! (liu & 1))
        for (j = 0; j < STRESS_AMOUNT; j++) {
            for (k = 0; k < STRESS_AMOUNT; k++) {
                v4p_transformClone(proto,
                                   stars[j][k],
                                   v4p_displayWidth * (1 + k - STRESS_AMOUNT / 2) / 2,
                                   v4p_displayWidth * (1 + j - STRESS_AMOUNT / 2),
                                   (j * k) / 2 + riu,
                                   1 + k % 12,
                                   256,
                                   256);
            }
        }

    // Use time-based animation
    riu += (STRESS_AMOUNT / 6) * timeFactor;
    iu += diu * timeFactor;
    return (liu < 0);
}

Boolean g4p_onFrame() {
    v4p_render();
    return success;
}

void g4p_onQuit() {
    v4pi_destroy();
}

int main(int argc, char** argv) {
    return g4p_main(argc, argv);
}
