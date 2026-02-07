#ifndef GAME_4_POCKET_H
#define GAME_4_POCKET_H
/**
 * G4P (Game 4 Pocket) Engine
 */
#include "v4p.h"
#include "collision.h"

// Button mapping enum
typedef enum {
    G4P_PEN = 0,    // Mouse button
    G4P_UP = 1,     // Up Arrow
    G4P_DOWN = 2,   // Down Arrow
    G4P_LEFT = 3,   // Left Arrow
    G4P_RIGHT = 4,  // Right Arrow
    G4P_SPACE = 5,  // Space
    G4P_SHIFT = 6,  // Shift
    G4P_ALT = 7,    // Alt
    G4P_CTRL = 8    // Ctrl
} G4pButton;

// Engine State
typedef struct g4pState_s {
    Boolean buttons[9];  // Button states
    V4pCoord xpen, ypen;
    UInt16 key;  // Legacy single key press (SDLK_* values)
} G4pState;

// Game 4 Pocket entry points
int g4p_setFramerate(int);
int g4p_main(int argc, char* argv[]);

// Game 4 Pocket input system callbacks
void g4pi_init();
void g4pi_destroy();

// Default V4P collision callback implementation of G4P
void g4p_onCollide(V4pCollisionLayer i1,
                   V4pCollisionLayer i2,
                   V4pCoord py,
                   V4pCoord x1,
                   V4pCoord x2,
                   V4pPolygonP p1,
                   V4pPolygonP p2);

// Game 4 Pocket output
extern G4pState g4p_state;
extern int g4p_framerate;
extern int g4p_avgFramePeriod;

// To be implemented by the caller
extern Boolean g4p_onInit(int quality, Boolean fullscreen);
extern Boolean g4p_onTick(Int32 deltaTime);
extern Boolean g4p_onFrame();
extern void g4p_onQuit();

#endif
