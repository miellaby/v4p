#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "game_engine.h"
#include "v4p.h"
#include "v4pi.h"
#include "v4pserial.h"
#include <stdio.h>
#define NUM_STARS 1000
#define STAR_DEPTH 100.0f
#define SPEED 0.011f
#define TWINKLE_SPEED 0.003f

typedef struct {
    float x, y, z;  // 3D position
    float size;  // Base star size
    float brightness;  // Current brightness (for twinkling)
    float twinkle_phase;  // Twinkle animation phase
    float twinkle_speed;  // Twinkle speed variation
    int color;  // Star color
} Star3D;

Star3D stars[NUM_STARS];
V4pPolygonP starPolygons[NUM_STARS];

// Initialize a star with random position
void initStar(Star3D* star) {
    star->x = (float) (rand() % (v4p_displayWidth * 40) - v4p_displayWidth * 20);
    star->y = (float) (rand() % (v4p_displayHeight * 40) - v4p_displayHeight * 20);
    star->z = (float) (rand() % (int) STAR_DEPTH);
    star->size = 0.2 * (0.5f + (float) (rand() % 5) * 0.2f);
    star->brightness = 0.1f + (float) (rand() % 20) * 0.01f;
    star->twinkle_phase = (float) (rand() % 100) * 0.01f;
    star->twinkle_speed = TWINKLE_SPEED * (0.5f + (float) (rand() % 10) * 0.1f);

    // Different colors for different star types
    int rnd = rand() % 10;
    if (rnd < 1) {
        star->color = V4P_BLUE;  // Blue-V4P_WHITE stars
    } else if (rnd < 3) {
        star->color = V4P_YELLOW;  // Yellow stars
    } else {
        star->color = V4P_WHITE;  // White stars
    }
}

// Project 3D coordinates to 2D screen space
void projectStar(Star3D* star, float* screenX, float* screenY, float* screenSize) {
    // Perspective projection with depth
    float factor = 8.0f / (8.0f + star->z);
    *screenX = star->x * factor;
    *screenY = star->y * factor;
    *screenSize = star->size * factor;
}

// Create a star-shaped polygon
V4pPolygonP createStarPolygon() {
    static V4pPolygonP poly = NULL;
    if (poly == NULL) {
        poly = v4p_new(V4P_ABSOLUTE, V4P_WHITE, 1);
        v4pPolygonDecodeSVGPath(poly,
                                "M 478.1,5  L 490.5,43.2 L 530.7,43.2 L 498.2,66.8 \
          L 510.6,105 L 478.1,81.4 L 445.6,105 L 458,66.8 \
          L 425.5,43.2 L 465.7,43.2 L 478.1,5 z",
                                4096);
        v4p_transform(poly, 0, 0, 0, 0, 16, 16);
        v4p_transform(poly, 0, 0, 0, 0, 128, 128);
        v4p_setAnchorToCenter(poly);
    }
    return v4p_addClone(poly);
}

Boolean g4p_onInit() {
    int i;

    // Seed random number generator
    srand(time(NULL));

    v4pi_init(1, 0);
    v4p_init();
    v4p_setView(-v4p_displayWidth * 10,
                -v4p_displayHeight * 10,
                v4p_displayWidth * 10,
                v4p_displayHeight * 10);
    v4p_setBGColor(V4P_BLACK);

    // Initialize all stars
    for (i = 0; i < NUM_STARS; i++) {
        initStar(&stars[i]);
        starPolygons[i] = createStarPolygon();
    }

    return success;
}

Boolean g4p_onTick(Int32 deltaTime) {
    int i;
    float screenX, screenY, screenSize;

    // Update all stars
    for (i = 0; i < NUM_STARS; i++) {
        // Move star towards viewer (decrease z)
        stars[i].z -= SPEED * deltaTime;
        float z = stars[i].z;

        // Twinkle animation
        stars[i].twinkle_phase += stars[i].twinkle_speed * deltaTime;
        if (stars[i].twinkle_phase > 1.0f) {
            stars[i].twinkle_phase -= 1.0f;
        }

        // Apply twinkle effect (sin wave for smooth brightness variation)
        float bright = stars[i].brightness + 0.3 * sinf(stars[i].twinkle_phase * 3.14159f * 2.0f);

        // If star passes the viewer, reset it to the back
        if (stars[i].z <= 0) {
            initStar(&stars[i]);
        }

        // Project 3D position to 2D screen
        projectStar(&stars[i], &screenX, &screenY, &screenSize);
        screenSize += bright / 8;
        // Update star position and appearance
        v4p_transform(starPolygons[i],
                      screenX,
                      screenY,
                      0,
                      0,
                      screenSize * 12000,
                      screenSize * 12000);

        // Adjust color based on brightness
        int brightColor = (stars[i].color + (int) (bright * 25)) % 255;
        v4p_setColor(starPolygons[i], brightColor);
        v4p_setLayer(starPolygons[i], 15 - (int) (15.f * stars[i].z / STAR_DEPTH));
    }

    return success;  // Run indefinitely
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