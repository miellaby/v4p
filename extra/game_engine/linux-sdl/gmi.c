#include <stdio.h>
#include "game_engine.h"
#include "SDL/SDL.h"
#include "gmi.h"

// get ticks
Int32 gmGetTicks() {
  return SDL_GetTicks();
}

// pause execution
void gmDelay(Int32 d) {
  SDL_Delay(d);
}

// Initialize the game engine
void gmiInit() {
  // SDL initialization is typically handled by the main application
  // This stub can be extended if needed for SDL-specific initialization
}

// Cleanup the game engine
void gmiDestroy() {
  // SDL cleanup is typically handled by the main application
  // This stub can be extended if needed for SDL-specific cleanup
}

// poll user events
int gmPollEvents() {
  int       rc = 0;  // return code

  SDL_Event event;  // event data

  while (!rc && SDL_PollEvent(&event)) {  // polling loop
    switch (event.type) {                 // event cases
      case SDL_QUIT:                      // time to quit
        rc = 1;
        break;

      case SDL_KEYDOWN:
        {
          SDLKey keypressed = event.key.keysym.sym;
          if (keypressed == SDLK_ESCAPE)
            rc = 1;
          gmState.key = (Uint16)keypressed;
          break;
        }
      case SDL_MOUSEBUTTONDOWN:
        gmState.buttons[0] = 1;
        break;

      case SDL_MOUSEBUTTONUP:
        gmState.buttons[0] = 0;
        break;

      case SDL_MOUSEMOTION:
        gmState.xpen = event.motion.x;
        gmState.ypen = event.motion.y;
        break;
    }  // switch
  }  // loop
  return rc;
}
