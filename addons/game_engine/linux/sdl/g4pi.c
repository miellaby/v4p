#include <stdio.h>
#include "g4p.h"
#include "SDL/SDL.h"
#include "g4pi.h"

// get ticks
Int32 g4p_getTicks() {
    return SDL_GetTicks();
}

// pause execution
void g4pi_delay(Int32 d) {
    SDL_Delay(d);
}

// Initialize the game engine
void g4pi_init() {
    // SDL initialization is typically handled by the main application
    // This stub can be extended if needed for SDL-specific initialization
}

// Cleanup the game engine
void g4pi_destroy() {
    // SDL cleanup is typically handled by the main application
    // This stub can be extended if needed for SDL-specific cleanup
}

// poll user events
int g4pi_pollEvents() {
    int rc = 0;  // return code

    SDL_Event event;  // event data

    while (! rc && SDL_PollEvent(&event)) {  // polling loop
        switch (event.type) {  // event cases
            case SDL_QUIT:  // time to quit
                rc = 1;
                break;

            case SDL_KEYDOWN:
                {
                    SDLKey keypressed = event.key.keysym.sym;
                    if (keypressed == SDLK_ESCAPE)
                        rc = 1;
                    g4p_state.key = (Uint16) keypressed;
                    break;
                }
            case SDL_MOUSEBUTTONDOWN:
                g4p_state.buttons[0] = 1;
                break;

            case SDL_MOUSEBUTTONUP:
                g4p_state.buttons[0] = 0;
                break;

            case SDL_MOUSEMOTION:
                g4p_state.xpen = event.motion.x;
                g4p_state.ypen = event.motion.y;
                break;
        }  // switch
    }  // loop
    return rc;
}
