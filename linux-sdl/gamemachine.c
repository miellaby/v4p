#include <stdio.h>
#include "gamemachine.h"
#include "v4p_ll.h"
#include "v4pi.h"
#include "SDL/SDL.h"

GmState gmMachineState;
int     gmFramerate = 30;

int gmSetFramerate(int new) {
	return (gmFramerate = new);
}

Boolean poll() {
    Boolean exit=0;
    SDL_Surface *screen;
    SDL_Event event;

      while (SDL_PollEvent(&event)) 
        { // polling

          switch (event.type) 
            { // event

            case SDL_QUIT:
              exit = 1;
              
              break;
            case SDL_KEYDOWN: {
              SDLKey keypressed = event.key.keysym.sym;
              if (keypressed == SDLK_ESCAPE) 
                exit = 1;
              
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
            
            }
        }
   return exit;
}

int gmMain(int argc, char* argv[])
{
    SDL_Surface *screen;
    Boolean exit=0;
    UInt32 excess, beforeTime, overSleepTime, afterTime,
       timeDiff, sleepTime;
    int  period = 1000 / gmFramerate;
    excess = 0;
    gmMachineState.buttons[0] = 0;

    gmOnInit();

    afterTime = SDL_GetTicks();
    sleepTime = 0;
    while (!exit)  { //main loop
      beforeTime = SDL_GetTicks();
      overSleepTime = (beforeTime - afterTime) - sleepTime;
      exit |= poll();
      exit |= gmOnIterate();
      exit |= gmOnFrame();

      period = 1000 / gmFramerate;
      afterTime = SDL_GetTicks();
      timeDiff = afterTime - beforeTime;
      sleepTime = (period - timeDiff) - overSleepTime;
      if (sleepTime <= 0) {
        excess -= sleepTime;
        sleepTime = 2;
      }
      SDL_Delay(sleepTime);

      while (excess > period) {
        exit |= poll();
        exit |= gmOnIterate();
        excess -= period;
      }
    }
    
    gmOnQuit();
    
    return EXIT_SUCCESS;
}
