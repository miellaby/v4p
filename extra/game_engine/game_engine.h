#ifndef GAME_4_POCKET_H
#define GAME_4_POCKET_H
#include "v4p_ll.h"
#include "collision.h"

/**
 * Game Engine State
 */

typedef struct g4pState_s {
    Boolean buttons[8];
    V4pCoord xpen, ypen;
    UInt16 key;
} G4pState;

// Game 4 Pocket entry points
int g4p_setFramerate(int);
int g4p_main(int argc, char* argv[]);

// Game 4 Pocket input system callbacks
void g4pi_init();
void g4pi_destroy();

// Game 4 Pocket output
extern G4pState g4p_state;
extern int g4p_framerate;
extern int g4p_avgFramePeriod;

// Collision system access
extern Collide collides[16];

// To be implemented by the caller
extern Boolean g4p_onInit();
extern Boolean g4p_onTick(Int32 deltaTime);
extern Boolean g4p_onFrame();
extern void g4p_onQuit();

#endif
