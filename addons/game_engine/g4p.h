#ifndef GAME_4_POCKET_H
#define GAME_4_POCKET_H
/**
 * G4P (Game 4 Pocket) Engine
 */
#include "v4p.h"
#include "collision.h"

// Engine State
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
extern Boolean g4p_onInit();
extern Boolean g4p_onTick(Int32 deltaTime);
extern Boolean g4p_onFrame();
extern void g4p_onQuit();

#endif
