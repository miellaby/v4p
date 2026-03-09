#include "g4pi.h"
#include "g4p.h"
#include "SDL/SDL.h"

#define MAX_EVENTS 32

// Event buffer for storing events between polls
typedef struct {
    G4pEvent events[MAX_EVENTS];
    int count;
    int read_ptr;
} EventBuffer;

static EventBuffer event_buffer = {0};

// Initialize the game engine
void g4pi_init() {
    // SDL initialization is typically handled by the main application
    // This stub can be extended if needed for SDL-specific initialization
}

// Helper function to add an event to the buffer
static void add_event(const G4pEvent* event) {
    if (event_buffer.count < MAX_EVENTS) {
        event_buffer.events[(event_buffer.read_ptr + event_buffer.count) % MAX_EVENTS] = *event;
        event_buffer.count++;
    }
}

// Helper function to convert SDL key to G4P key code
static G4pKeyCode sdl_to_g4p_key(SDLKey sdl_key) {
    switch (sdl_key) {
        case SDLK_RETURN: return G4P_KEY_RETURN;
        case SDLK_ESCAPE: return G4P_KEY_ESCAPE;
        case SDLK_BACKSPACE: return G4P_KEY_BACKSPACE;
        case SDLK_TAB: return G4P_KEY_TAB;
        case SDLK_SPACE: return G4P_KEY_SPACE;
        case SDLK_UP: return G4P_KEY_UP;
        case SDLK_DOWN: return G4P_KEY_DOWN;
        case SDLK_LEFT: return G4P_KEY_LEFT;
        case SDLK_RIGHT: return G4P_KEY_RIGHT;
        case SDLK_DELETE: return G4P_KEY_DELETE;
        case SDLK_HOME: return G4P_KEY_HOME;
        case SDLK_END: return G4P_KEY_END;
        case SDLK_PAGEUP: return G4P_KEY_PAGEUP;
        case SDLK_PAGEDOWN: return G4P_KEY_PAGEDOWN;
        case SDLK_INSERT: return G4P_KEY_INSERT;
        case SDLK_LSHIFT: return G4P_KEY_LSHIFT;
        case SDLK_RSHIFT: return G4P_KEY_RSHIFT;
        case SDLK_LCTRL: return G4P_KEY_LCTRL;
        case SDLK_RCTRL: return G4P_KEY_RCTRL;
        case SDLK_LALT: return G4P_KEY_LALT;
        case SDLK_RALT: return G4P_KEY_RALT;
        default: return (G4pKeyCode)sdl_key; // Return the raw SDL key code for unmapped keys
    }
}

// Poll a single event from the event buffer
bool g4p_pollEvent(G4pEvent* event) {
    if (event_buffer.count > 0) {
        *event = event_buffer.events[event_buffer.read_ptr];
        event_buffer.read_ptr = (event_buffer.read_ptr + 1) % MAX_EVENTS;
        event_buffer.count--;
        return true;
    }
    return false;
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
                    
                    // Create detailed event
                    {
                        G4pEvent g4p_event;
                        g4p_event.type = G4P_EVENT_KEY_DOWN;
                        g4p_event.data.key.key = sdl_to_g4p_key(keypressed);
                        g4p_event.data.key.ctrl = (event.key.keysym.mod & KMOD_CTRL) != 0;
                        g4p_event.data.key.shift = (event.key.keysym.mod & KMOD_SHIFT) != 0;
                        g4p_event.data.key.alt = (event.key.keysym.mod & KMOD_ALT) != 0;
                        add_event(&g4p_event);
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
                
                // Create detailed event
                {
                    G4pEvent g4p_event;
                    g4p_event.type = G4P_EVENT_KEY_UP;
                    g4p_event.data.key.key = sdl_to_g4p_key(event.key.keysym.sym);
                    g4p_event.data.key.ctrl = (event.key.keysym.mod & KMOD_CTRL) != 0;
                    g4p_event.data.key.shift = (event.key.keysym.mod & KMOD_SHIFT) != 0;
                    g4p_event.data.key.alt = (event.key.keysym.mod & KMOD_ALT) != 0;
                    add_event(&g4p_event);
                }
                
                break;

            case SDL_MOUSEBUTTONDOWN:
                // Handle mouse wheel events (buttons 4 and 5)
                if (event.button.button == 4 || event.button.button == 5) {
                    // Mouse wheel scroll event
                    G4pEvent g4p_event;
                    g4p_event.type = G4P_EVENT_MOUSE_WHEEL;
                    g4p_event.data.wheel.x = 0; // Horizontal scroll
                    g4p_event.data.wheel.y = (event.button.button == 4) ? 1.0f : -1.0f; // Vertical scroll
                    add_event(&g4p_event);
                }
                // Handle regular mouse buttons
                else if (event.button.button == SDL_BUTTON_LEFT || 
                         event.button.button == SDL_BUTTON_MIDDLE || 
                         event.button.button == SDL_BUTTON_RIGHT) {
                    g4p_state.buttons[G4P_PEN] = 1;
                    
                    // Create detailed event
                    G4pEvent g4p_event;
                    g4p_event.type = G4P_EVENT_MOUSE_DOWN;
                    g4p_event.data.mouse.button = event.button.button;
                    g4p_event.data.mouse.x = event.button.x;
                    g4p_event.data.mouse.y = event.button.y;
                    g4p_event.data.mouse.clicks = 1; // SDL 1.2 doesn't have clicks field
                    add_event(&g4p_event);
                }
                
                break;

            case SDL_MOUSEBUTTONUP:
                // Only handle regular mouse buttons for pen state and mouse up events
                if (event.button.button == SDL_BUTTON_LEFT || 
                    event.button.button == SDL_BUTTON_MIDDLE || 
                    event.button.button == SDL_BUTTON_RIGHT) {
                    g4p_state.buttons[G4P_PEN] = 0;
                    
                    // Create detailed event
                    G4pEvent g4p_event;
                    g4p_event.type = G4P_EVENT_MOUSE_UP;
                    g4p_event.data.mouse.button = event.button.button;
                    g4p_event.data.mouse.x = event.button.x;
                    g4p_event.data.mouse.y = event.button.y;
                    g4p_event.data.mouse.clicks = 1; // SDL 1.2 doesn't have clicks field
                    add_event(&g4p_event);
                }
                // Wheel buttons don't generate mouse up events
                
                break;

            case SDL_MOUSEMOTION:
                g4p_state.xpen = event.motion.x;
                g4p_state.ypen = event.motion.y;
                
                // Create detailed event
                {
                    G4pEvent g4p_event;
                    g4p_event.type = G4P_EVENT_MOUSE_MOVE;
                    g4p_event.data.motion.x = event.motion.x;
                    g4p_event.data.motion.y = event.motion.y;
                    add_event(&g4p_event);
                }
                
                break;
        }  // switch
    }  // loop
    return rc;
}
