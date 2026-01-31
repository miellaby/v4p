# Project

C-based 2D vector graphic engine targeting tiny MPU with nothing more than a video ram buffer.

## Build

Linux is the development platform with SDL1.2 backend as a video buffer.

    make libv4p.a # builds V4P with 
    MODE=debug make clean all # rebuild with debug traces
    BACKEND=xlib make libpv4.a # alternative backend (xlib, fbdev or libdrm are available)

Don't invoke the sub-makefiles. Run 'make' from the top dir.

## Debug

v4p_error and v4p_debug may be used for tracing, but v4p_debug is enabled only with MODE=debug build.

## Code

- create plain polygons and circles and ensure they are added to the scene.
- v4p_transform() to move/rotate/zoom these objects.
- v4p_render() to render the scene
- use the "g4p" engine (addons/g4p/libg4p.a) to code an interactive animation/game. g4p runs a input>logic>draw infinite loop (esc to exit). You only have to code the app logic in onTick() callback.


# Pattern to create animated objects

Warning: Transformations are cumulative by default (transformations "in place").

By cloning an existing polygon (out of scene), one gets not-cumulative transformations (transformations of clones are computed against their parents.

// Create a star-shaped polygon
V4pPolygonP createStar() {
    static V4pPolygonP poly = NULL; // proto
    if (poly == NULL) { // proto initialization
        poly = v4p_new(V4P_ABSOLUTE, V4P_WHITE, 1);
        v4pPolygonDecodeSVGPath(poly,
                                "M 478.1,5  L 490.5,43.2 L 530.7,43.2 L 498.2,66.8 \
          L 510.6,105 L 478.1,81.4 L 445.6,105 L 458,66.8 \
          L 425.5,43.2 L 465.7,43.2 L 478.1,5 z",
                                128); // SVG coordinates scaled to 128/256 (50%)
        v4p_setAnchorToCenter(poly);
    }
    // return a transformable copy (already added to the scene)
    return v4p_addClone(poly);
}

# "game loop" boilerplate
..
#include "v4p.h"
#include "g4p.h"
..

Boolean g4p_onInit() {
    v4pi_init(V4P_QUALITY_NORMAL, V4P_UX_NORMAL);
    v4p_init();
    v4p_setBGColor(V4P_BLACK);

    // Prepare the scene, add polys (use the cloning pattern)
    return success;
}

Boolean g4p_onTick(Int32 deltaTime) {
    // read inputs
    // move stuff (transform)
}

Boolean g4p_onFrame() {
    return v4p_render();
}

void g4p_onQuit() {
    v4pi_destroy();
}

int main(int argc, char** argv) {
    return g4p_main(argc, argv);
}

## Input

g4p gather user inputs

typedef struct g4pState_s {
    Boolean buttons[8];
    V4pCoord xpen, ypen;
    UInt16 key;
} G4pState;

... where key is the currently pressed key code (SDL_Key enum).

