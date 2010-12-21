#include <stdio.h>
#include "gamemachine.h"
#include "v4p_ll.h"
#include "v4pi.h"
#include "SDL/SDL.h"

// The machine states holds basic up-to-date data
GmState gmMachineState;

//framerate stuff
#define DEFAULT_FRAMERATE 30
#define MAX_PERIOD (5 * 60000)
int     gmFramerate = DEFAULT_FRAMERATE;
static  int gmPeriod = 1000 / DEFAULT_FRAMERATE; // private

// change the framerate
int gmSetFramerate(int new) {
  gmFramerate = new;
  gmPeriod = (new > 0 ? 1000 / gmFramerate : MAX_PERIOD);
	return (new);
}

// poll user events
Boolean gmPollEvents() {
  Boolean rc = 0; // return code

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

// Machine main function
int gmMain(int argc, char* argv[])
{
    Boolean rc = 0;
    Int32 excess, beforeTime, overSleepTime, afterTime,
       timeDiff, sleepTime;

    // reset machine state
    gmMachineState.buttons[0] = 0;

    // Init call-back
    if (gmOnInit())
      return failure;

    afterTime = SDL_GetTicks();
    sleepTime = 0;
    excess = 0;
    while (!rc)  { //main machine loop
      // w/ clever hackery to handle properly performance drops
      beforeTime = SDL_GetTicks();
      overSleepTime = (beforeTime - afterTime) - sleepTime;

      // poll user events
      rc |= gmPollEvents();
      // process scene iteration
      rc |= gmOnIterate();
      // render a frame
      rc |= gmOnFrame();

      // maximize frame rates and detect performance drops
      afterTime = SDL_GetTicks();
      timeDiff = afterTime - beforeTime;
      sleepTime = (gmPeriod - timeDiff) - overSleepTime;
      if (sleepTime <= 0) {
        excess -= sleepTime;
        sleepTime = 2;
      }
      SDL_Delay(sleepTime);

      // when framerate is low, one repeats non-display steps
      while (excess > gmPeriod) {
        rc |= gmPollEvents();
        rc |= gmOnIterate();
        excess -= gmPeriod;
      }
    }

    // we're done.    
    gmOnQuit();
    
    return success;
}
