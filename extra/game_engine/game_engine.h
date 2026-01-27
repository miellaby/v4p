#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H
#include "v4p_ll.h"

/*****************************************
 * Game Engine State
 ****************************************/

typedef struct gmState_s {
  Boolean buttons[8];
  Coord   xpen, ypen;
  UInt16  key;
} GmState;

// Game engine entry points
int            gmSetFramerate(int);
int            gmMain(int argc, char *argv[]);

// Game engine input system callbacks
void           gmiInit();
void           gmiDestroy();

// Game engine output
extern GmState gmState;
extern int     gmFramerate;
extern int     gmAvgFramePeriod;

// To be implemented by the caller
extern Boolean gmOnInit();
extern Boolean gmOnIterate();
extern Boolean gmOnFrame();
extern void    gmOnQuit();

#endif
