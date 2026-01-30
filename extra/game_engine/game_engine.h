#ifndef GAME_4_POCKET_H
#define GAME_4_POCKET_H
#include "v4p_ll.h"

/**
 * Game Engine State
 */

typedef struct g4pState_s {
    Boolean buttons[8];
    Coord xpen, ypen;
    UInt16 key;
} G4pState;

// Game 4 Pocket entry points
int g4pSetFramerate(int);
int g4pMain(int argc, char* argv[]);

// Game 4 Pocket input system callbacks
void g4piInit();
void g4piDestroy();

// Game 4 Pocket output
extern G4pState g4pState;
extern int g4pFramerate;
extern int g4pAvgFramePeriod;

// To be implemented by the caller
extern Boolean g4pOnInit();
extern Boolean g4pOnTick(Int32 deltaTime);
extern Boolean g4pOnFrame();
extern void g4pOnQuit();

#endif
