# Project

C-based 2D vector graphic engine.

It targets tiny MPU with nothing more than a video ram buffer. Integer maths. Binary ops. No dependencies.

## Dev

You avoid guards and prefer a crash-early design pattern.

## Build

```bash
# Linux build (default) with SDL1.2 backend as a video buffer

make

# Debug build
make DEBUG=1

# Different backend
make BACKEND=xlib

# Verbose make (ALWAYS USE IT WHEN CODING)
make V=1

# Install to system
make install

# build demos
make demos # or demos/single_demo
```

## Build Options

- `DEBUG=1`: Enable debug build with symbols and debug macros
- `BACKEND=sdl|xlib|fbdev|drm`: Select Linux backend (default: sdl)
- `TARGET=linux|palmos|esp32`: Select platform (default: linux)
- `V=1`: Verbose output showing all commands
- `PREFIX=/path`: Custom installation prefix (default: /usr/local)

## Targets

- `all`: Build everything (default)
- `libv4p.a`: Build core library
- `addons`: Build all addons
- `demos`: Build all demos
- `install`: Install to system
- `uninstall`: Remove installation
- `clean`: Clean build artifacts
- `help`: Show usage information

## Debug

v4p_error and v4p_debug may be used for tracing, but v4p_debug is enabled only with DEBUG=1 build.
Build with V=1 to see the compilation steps.

## Coding a demo/app with v4p AND g4p

- create plain polygons and circles and ensure they are added to the scene.
- v4p_transform() to move/rotate/zoom these objects.
- v4p_render() to render the scene
- use the companion "g4p" engine (addons/g4p/libg4p.a) to code an interactive animation/game. g4p runs a input>logic>draw infinite loop (esc to exit). You only have to code the app logic in onTick() callback.


# Pattern to create animated objects

Warning: Transformations are cumulative by default (because "in place").

By creating an existing polygon out of scene and cloning it, you get polygons whose transformations aren't cumulative (transformations computed against their parents).

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
    // return a transformable copy (added to the scene)
    return v4p_addClone(poly);
}

# game/app loop boilerplate
..
#include "v4p.h"
#include "g4p.h"
..

Boolean g4p_onInit() {
    v4p_init();
    v4p_setBGColor(V4P_BLACK);
    // add polys to the scene (use the cloning pattern to make transform easier)
    return success;
}

Boolean g4p_onTick(Int32 deltaTime) {
    // read inputs
    // transform stuff
}

Boolean g4p_onFrame() {
    return v4p_render();
}

void g4p_onQuit() {
    v4p_quit();
}

int main(int argc, char** argv) {
    return g4p_main(argc, argv);
}

## User Inputs

They're in

typedef struct g4pState_s {
    Boolean buttons[8];
    V4pCoord xpen, ypen;
    UInt16 key;
} G4pState;

... where key is the currently pressed key code (SDL_Key enum).

