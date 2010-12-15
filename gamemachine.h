#ifndef GAMEMACHINE_H
#define GAMEMACHINE_H
#include "v4p_ll.h"

/*****************************************
 * Game Machine State
 ****************************************/

typedef struct gmState_s {
 Boolean buttons[8];
 Coord xpen, ypen;
} GmState;

extern GmState gmMachineState;

extern Boolean gmOnInit() ;
extern Boolean gmOnIterate() ;
extern void    gmOnQuit() ;

int gmMain(int argc, char* argv[]);
#endif
