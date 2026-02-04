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
                    
                    // Map keys to buttons array
                    switch (keypressed) {
                        case SDLK_UP:     g4p_state.buttons[G4P_UP] = 1; break;
                        case SDLK_DOWN:   g4p_state.buttons[G4P_DOWN] = 1; break;
                        case SDLK_LEFT:   g4p_state.buttons[G4P_LEFT] = 1; break;
                        case SDLK_RIGHT:  g4p_state.buttons[G4P_RIGHT] = 1; break;
                        case SDLK_SPACE:  g4p_state.buttons[G4P_SPACE] = 1; break;
                        case SDLK_LSHIFT:
                        case SDLK_RSHIFT: g4p_state.buttons[G4P_SHIFT] = 1; break;
                        case SDLK_LALT:
                        case SDLK_RALT:   g4p_state.buttons[G4P_ALT] = 1; break;
                        case SDLK_LCTRL:
                        case SDLK_RCTRL:  g4p_state.buttons[G4P_CTRL] = 1; break;
                        default:
                            // Unmapped key, store in key field
                            g4p_state.key = (Uint16) keypressed;
                    }
                    break;
                }
            case SDL_KEYUP:
                // Clear key buttons on key up
                switch (event.key.keysym.sym) {
                    case SDLK_UP:     g4p_state.buttons[G4P_UP] = 0; break;
                    case SDLK_DOWN:   g4p_state.buttons[G4P_DOWN] = 0; break;
                    case SDLK_LEFT:   g4p_state.buttons[G4P_LEFT] = 0; break;
                    case SDLK_RIGHT:  g4p_state.buttons[G4P_RIGHT] = 0; break;
                    case SDLK_SPACE:  g4p_state.buttons[G4P_SPACE] = 0; break;
                    case SDLK_LSHIFT:
                    case SDLK_RSHIFT: g4p_state.buttons[G4P_SHIFT] = 0; break;
                    case SDLK_LALT:
                    case SDLK_RALT:   g4p_state.buttons[G4P_ALT] = 0; break;
                    case SDLK_LCTRL:
                    case SDLK_RCTRL:  g4p_state.buttons[G4P_CTRL] = 0; break;
                    default: // Unmapped key, clear key field if it matches
                        if (g4p_state.key == event.key.keysym.sym)
                            g4p_state.key = 0;
                        break;
                }
                break;

            case SDL_MOUSEBUTTONDOWN:
                g4p_state.buttons[G4P_PEN] = 1;
                break;

            case SDL_MOUSEBUTTONUP:
                g4p_state.buttons[G4P_PEN] = 0;
                break;

            case SDL_MOUSEMOTION:
                g4p_state.xpen = event.motion.x;
                g4p_state.ypen = event.motion.y;
                break;
        }  // switch
    }  // loop
    return rc;
}
