#ifndef GMI_H
#define GMI_H
#include "v4p_ll.h"

/*
 * Game Machine Implementation
 */

// Initialize the gamemachine backend
void  gmiInit();

// Cleanup the gamemachine backend
void  gmiDestroy();

// get ticks in milliseconds
Int32 gmGetTicks();

// pause execution for milliseconds
void  gmDelay(Int32 d);

// poll user events
int   gmPollEvents();

#endif
