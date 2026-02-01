#include <stdio.h>
#include "g4p.h"
#include "v4p_ll.h"
#include "v4pi.h"
#include "g4pi.h"
#include "collision.h"

// The game 4 pocket states holds basic up-to-date data
G4pState g4p_state;

// framerate stuff
#define DEFAULT_FRAMERATE 60
#define MAX_PERIOD (5 * 60000)
#define MAX_CATCHUP 5
int g4p_framerate = DEFAULT_FRAMERATE;
int g4p_avgFramePeriod = 1000 / DEFAULT_FRAMERATE;
static int g4p_period = 1000 / DEFAULT_FRAMERATE;  // private

// change the framerate
int g4p_setFramerate(int new) {
    g4p_framerate = new;
    g4p_period = (new > 0 ? 1000 / g4p_framerate : MAX_PERIOD);
    return (new);
}

// Game 4 Pocket main function
int g4p_main(int argc, char* argv[]) {
    Boolean rc = 0;
    Int32 beforeTime, afterTime, timeDiff, sleepTime, overSleepTime;
    Int32 lastTickTime = 0;
    Int32 deltaTime = 0;

    // reset game 4 pocket state
    g4p_state.buttons[0] = 0;
    g4p_state.key = 0;

    // Initialize
    g4pi_init();

    // Initialize collision system
    g4p_initCollide();

    // Init call-back
    if (g4p_onInit())
        return failure;

    lastTickTime = g4p_getTicks();
    while (! rc) {  // main game 4 pocket loop
        // Get current time and calculate delta since last tick
        beforeTime = g4p_getTicks();
        deltaTime = beforeTime - lastTickTime;
        lastTickTime = beforeTime;

        // process late ticks with period = max period (up to 5 late ticks max)
        int catchup = 0;
        while (! rc && deltaTime > g4p_period && catchup++ < MAX_CATCHUP) {
            rc |= g4p_onTick(g4p_period);
            deltaTime -= g4p_period;
        }

        // process current tick with remaining elapsed time
        if (! rc) {
            rc |= g4p_onTick(deltaTime);
            deltaTime = 0;
        }

        // poll user events
        rc |= g4pi_pollEvents();

        // Reset collision data before rendering
        g4p_resetCollide();
        
        // render a frame
        rc |= g4p_onFrame();
        
        // Finalize collision data after rendering
        g4p_finalizeCollide();

        // calculate sleep time to maintain target frame rate
        afterTime = g4p_getTicks();
        timeDiff = afterTime - beforeTime;
        sleepTime = (g4p_period - timeDiff);
        if (sleepTime <= 2) {
            sleepTime = 2;  // minimum sleep to prevent busy waiting
        }
        g4pi_delay(sleepTime);
        g4p_avgFramePeriod = (3 * g4p_avgFramePeriod + timeDiff + sleepTime) / 4;
        v4pi_debug("fps=%.1f", 1000.f / g4p_avgFramePeriod);
    }

    // we're done.
    g4p_onQuit();

    // Cleanup
    g4pi_destroy();

    return success;
}
