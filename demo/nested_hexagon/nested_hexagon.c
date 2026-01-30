#include "game_engine.h"
#include "v4p.h"
#include "v4pi.h"
#include "lowmath.h"  // For iabs() and computeCosSin() functions

#define NUM_STARS 6

PolygonP hexagon;
PolygonP stars[NUM_STARS];

// Function to create a star
PolygonP create_star(Coord size, Color color) {
    PolygonP star = v4p_new(absolute, color, 10);

    // Create a 5-pointed star using v4p's trigonometric system
    int i;
    Coord center_x = 0, center_y = 0;

    for (i = 0; i < 10; i++) {
        // Convert to v4p angle format (0-511 where 512=360°)
        // 36° per point, start from top (-90° = 270°)
        int angle = ((i * 36) - 90 + 360) % 360;  // Ensure positive angle in degrees
        int v4p_angle = (angle * 512) / 360;  // Convert degrees to v4p format (0-511)

        computeCosSin(v4p_angle);
        Coord radius
            = (i % 2 == 0) ? size : size * 0.4f;  // Alternate between outer and inner points
        Coord x = center_x + ((radius * lwmCosa) >> 8);  // Scale by 256 and shift right
        Coord y = center_y + ((radius * lwmSina) >> 8);
        v4p_addPoint(star, x, y);
    }

    return star;
}

// Function to create a regular hexagon
PolygonP create_hexagon(Coord size, Color color) {
    PolygonP hex = v4p_new(absolute, color, 5);

    int i;
    Coord center_x = 0, center_y = 0;

    for (i = 0; i < 6; i++) {
        // 60° per point, start from top (-90°)
        int angle = ((i * 60) - 90 + 360) % 360;  // Ensure positive angle in degrees
        int v4p_angle = (angle * 512) / 360;  // Convert degrees to v4p format (0-511)

        computeCosSin(v4p_angle);
        Coord x = center_x + ((size * lwmCosa) >> 8);
        Coord y = center_y + ((size * lwmSina) >> 8);
        v4p_addPoint(hex, x, y);
    }

    return hex;
}

Boolean g4pOnInit() {
    v4p_init();
    v4pDisplayInit(1, 0);  // Normal quality, windowed
    v4p_setBGColor(black);  // Black background

    // Create the main hexagon (this will be the parent)
    Coord hexagon_size = v4pDisplayWidth / 10;
    hexagon = create_hexagon(hexagon_size, maroon);
    v4p_setAnchorToCenter(hexagon);

    // Create stars at each corner of the hexagon
    for (int i = 0; i < NUM_STARS; i++) {
        // 60° per star, start from top (-90°)
        int angle = ((i * 60) - 90 + 360) % 360;  // Ensure positive angle in degrees
        int v4p_angle = (angle * 512) / 360;  // Convert degrees to v4p format (0-511)

        // Create star
        stars[i] = create_star(hexagon_size / 3, red + (i * 4));  // Different colors for each star
        v4p_setAnchorToCenter(stars[i]);

        // Position star at hexagon corner using v4p trig
        computeCosSin(v4p_angle);
        Coord star_x = (hexagon_size * lwmCosa) >> 8;
        Coord star_y = (hexagon_size * lwmSina) >> 8;

        // Add star as a child of the hexagon
        v4p_addSub(hexagon, stars[i]);
        v4p_transform(stars[i], star_x, star_y, 0, 0, 256, 256);
    }

    // clone the hexagon
    hexagon = v4p_addClone(hexagon);
    return success;
}

Boolean g4pOnTick(Int32 deltaTime) {
    static int elapsedTime = 0;
    elapsedTime += deltaTime;

    // Rotate and pulse the entire hexagon (including all child stars)
    int rotation_angle = (elapsedTime / 16) % 512;  // Smooth rotation based on time

    // Use triangle wave pulsing (same as zoom_test)
    int phase = (elapsedTime / 2) % 256;  // 0 to 255
    int zoom_factor = 128 + (255 - iabs(phase - 128));  // Creates triangle wave: 128->0->128

    v4p_transform(hexagon,
                  v4pDisplayWidth / 2,
                  v4pDisplayHeight / 2,
                  rotation_angle,
                  0,
                  zoom_factor,
                  zoom_factor);
    return success;
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