#ifndef GMI_H
#define GMI_H
#include "v4p_ll.h"

/*
 * Game Engine Implementation
 */

// Initialize the game engine backend
void  gmiInit();

// Cleanup the game engine backend
void  gmiDestroy();

// get ticks in milliseconds
Int32 gmGetTicks();

// pause execution for milliseconds
void  gmDelay(Int32 d);

// poll user events
int   gmPollEvents();

#endif
