#ifndef GAMEMACHINE_H
#define GAMEMACHINE_H
#include "v4p_ll.h"

/*****************************************
 * Game Machine State
 ****************************************/

typedef struct gmState_s {
  Boolean buttons[8];
  Coord   xpen, ypen;
} GmState;

// Game machine entry points
int            gmSetFramerate(int);
int            gmMain(int argc, char *argv[]);

// Game machine input system callbacks
void           gmiInit();
void           gmiDestroy();

// Game machine output
extern GmState gmMachineState;
extern int     gmFramerate;
extern int     gmAvgFramePeriod;

// To be implemented by the caller
extern Boolean gmOnInit();
extern Boolean gmOnIterate();
extern Boolean gmOnFrame();
extern void    gmOnQuit();

#endif
