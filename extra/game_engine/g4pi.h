#ifndef GAME_4_POCKET_I_H
#define GAME_4_POCKET_I_H
#include "v4p_ll.h"

/**
 * Game 4 Pocket Implementation
 */

// Initialize the game 4 pocket backend
void g4piInit();

// Cleanup the game 4 pocket backend
void g4piDestroy();

// get ticks in milliseconds
Int32 g4pGetTicks();

// pause execution for milliseconds
void g4pDelay(Int32 d);

// poll user events
int g4pPollEvents();

#endif
