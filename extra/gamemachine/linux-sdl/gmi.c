#include <stdio.h>
#include "gamemachine.h"
#include "SDL/SDL.h"

// get ticks
Int32 gmGetTicks() {
	return SDL_GetTicks();
}

// pause execution
void gmDelay(Int32 d) {
	SDL_Delay(d);
}

// poll user events
int gmPollEvents() {
  int rc = 0; // return code

  SDL_Event event; // event data

  while (!rc && SDL_PollEvent(&event))  { // polling loop
    switch (event.type) { // event cases
       case SDL_QUIT: // time to quit
         rc = 1;
         break;

       case SDL_KEYDOWN: {
         SDLKey keypressed = event.key.keysym.sym;
         if (keypressed == SDLK_ESCAPE) 
            rc = 1;
            break;
         }
       case SDL_MOUSEBUTTONDOWN:
          gmMachineState.buttons[0] = 1;
          break;

       case SDL_MOUSEBUTTONUP:
          gmMachineState.buttons[0] = 0;
          break;

       case SDL_MOUSEMOTION:
          gmMachineState.xpen = event.motion.x;
          gmMachineState.ypen = event.motion.y;
          break;
       } // switch
    } // loop
   return rc;
}
