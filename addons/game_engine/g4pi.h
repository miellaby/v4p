#ifndef GAME_4_POCKET_I_H
#define GAME_4_POCKET_I_H
#include "v4p_platform.h"

/**
 * Game 4 Pocket Implementation
 */

// Initialize the game 4 pocket backend
void g4pi_init();

// Cleanup the game 4 pocket backend
void g4pi_destroy();

// poll user events
int g4pi_pollEvents();

#endif
