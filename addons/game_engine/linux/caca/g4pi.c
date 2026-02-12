#include <stdio.h>
#include <time.h>
#include <caca.h>
#include "g4p.h"
#include "g4pi.h"
#include "v4pi.h"

// get ticks in milliseconds
Int32 g4p_getTicks() {
    // libcaca doesn't have a direct tick function, so we use standard time
    // This is a simplified implementation
    static struct timespec start = { 0, 0 };
    struct timespec now;

    clock_gettime(CLOCK_MONOTONIC, &now);

    if (start.tv_sec == 0 && start.tv_nsec == 0) {
        start = now;
        return 0;
    }

    return (Int32) ((now.tv_sec - start.tv_sec) * 1000 + (now.tv_nsec - start.tv_nsec) / 1000000);
}

// pause execution
void g4pi_delay(Int32 d) {
    // Simple busy wait for libcaca backend
    // Note: This is not ideal for production, but works for demo purposes
    struct timespec ts;
    ts.tv_sec = d / 1000;
    ts.tv_nsec = (d % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

// Initialize the game engine
void g4pi_init() {
}

// Cleanup the game engine
void g4pi_destroy() {
    // libcaca cleanup is handled by the main application's v4pi_destroy
}



// poll user events
int g4pi_pollEvents() {
    int rc = 0;  // return code
    
    // Key release workaround state
    static UInt16 timeout = 0;

    caca_event_t ev;

    // Get the current display from the v4pi context
    caca_display_t* display = *(caca_display_t**) v4pi_context;

    if (! display) {
        // Display not available yet, no events
        return 0;
    }

    if (caca_get_event(display, CACA_EVENT_ANY, &ev, 0)) {
        switch (caca_get_event_type(&ev)) {
            case CACA_EVENT_QUIT:
                rc = 1;  // time to quit
                break;

            case CACA_EVENT_KEY_PRESS:
                {
                    int key = caca_get_event_key_ch(&ev);
                    timeout = 10;

                    // Map keys to buttons array
                    switch (key) {
                        case CACA_KEY_UP:
                            g4p_state.buttons[G4P_UP] = 1;
                            break;
                        case CACA_KEY_DOWN:
                            g4p_state.buttons[G4P_DOWN] = 1;
                            break;
                        case CACA_KEY_LEFT:
                            g4p_state.buttons[G4P_LEFT] = 1;
                            break;
                        case CACA_KEY_RIGHT:
                            g4p_state.buttons[G4P_RIGHT] = 1;
                            break;
                        case CACA_KEY_ESCAPE:
                            rc = 1;  // ESC to quit
                            break;
                        case ' ':
                            g4p_state.buttons[G4P_SPACE] = 1;
                            break;
                        default:
                            // Unmapped key, store in key field
                            g4p_state.key = key;
                            break;
                    }
                    break;
                }

            case CACA_EVENT_KEY_RELEASE:
                // libcaca with ncurses doesn't provide key release events in terminal
                // This case is kept for completeness but won't be triggered
                switch (caca_get_event_key_ch(&ev)) {
                    case CACA_KEY_UP:
                        g4p_state.buttons[G4P_UP] = 0;
                        break;
                    case CACA_KEY_DOWN:
                        g4p_state.buttons[G4P_DOWN] = 0;
                        break;
                    case CACA_KEY_LEFT:
                        g4p_state.buttons[G4P_LEFT] = 0;
                        break;
                    case CACA_KEY_RIGHT:
                        g4p_state.buttons[G4P_RIGHT] = 0;
                        break;
                    case ' ':
                        g4p_state.buttons[G4P_SPACE] = 0;
                        break;
                    default:
                        if (g4p_state.key == (UInt16) caca_get_event_key_ch(&ev)) g4p_state.key = 0;
                }
                break;

            case CACA_EVENT_MOUSE_PRESS:
                g4p_state.buttons[G4P_PEN] = 1;
                // Scale mouse coordinates from characters to pixels
                // Assuming 8x16 pixels per character
                g4p_state.xpen = ev.data.mouse.x * 2;
                g4p_state.ypen = ev.data.mouse.y * 3;
                break;

            case CACA_EVENT_MOUSE_RELEASE:
                g4p_state.buttons[G4P_PEN] = 0;
                break;

            case CACA_EVENT_MOUSE_MOTION:
                // Scale mouse coordinates from characters to pixels
                g4p_state.xpen = ev.data.mouse.x * 2;
                g4p_state.ypen = ev.data.mouse.y * 3;
                break;

            case CACA_EVENT_RESIZE:
                // Handle resize if needed
                break;

            case CACA_EVENT_NONE:
            case CACA_EVENT_ANY:
                // Ignore these event types
                break;
        }  // switch
    } else { // No event received
        if (timeout > 0) {
            timeout--;
        }

        // Key release workaround: auto-release last pressed key
        if (timeout == 0) {
            // just clear all buttons after timeout to avoid sticking keys, since we don't get real key release events
            for (int i = 0; i < 9; i++) {
                g4p_state.buttons[i] = 0;
            }
            g4p_state.key = 0;
        }
    }  // if event

    return rc;
}