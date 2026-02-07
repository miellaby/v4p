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

// Default collision callback
void g4p_onCollide(V4pCollisionLayer i1,
                   V4pCollisionLayer i2,
                   V4pCoord py,
                   V4pCoord x1,
                   V4pCoord x2,
                   V4pPolygonP p1,
                   V4pPolygonP p2) {
    // Compute collision point (midpoint of the collision segment)
    V4pCoord collision_x = (x1 + x2) / 2;
    V4pCoord collision_y = py;

    // Store collision point for this polygon pair
    g4p_addCollisionPoint(p1, p2, collision_x, collision_y);
}

// Game 4 Pocket main function
int g4p_main(int argc, char* argv[]) {
        Boolean rc = 0;
        Int32 beforeTime, afterTime, timeDiff, sleepTime, overSleepTime;
        Int32 lastTickTime = 0;
        Int32 deltaTime = 0;

        // reset game 4 pocket state
        for (int i = 0; i < 9; i++) {
            g4p_state.buttons[i] = 0;
        }
        g4p_state.key = 0;

        // Initialize
        g4pi_init();

        // Initialize collision points system with a reasonable table size
        g4p_initCollisions(64);

        // Set default callback
        v4p_setCollisionCallback(g4p_onCollide);

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
            g4p_resetCollisions();

            // render a frame
            rc |= g4p_onFrame();

            // Finalize collision data after rendering
            g4p_finalizeCollisions();

            // calculate sleep time to maintain target frame rate
            afterTime = g4p_getTicks();
            timeDiff = afterTime - beforeTime;
            sleepTime = (g4p_period - timeDiff);
            if (sleepTime <= 2) {
                sleepTime = 2;  // minimum sleep to prevent busy waiting
            }
            g4pi_delay(sleepTime);
            g4p_avgFramePeriod = (3 * g4p_avgFramePeriod + timeDiff + sleepTime) / 4;
            v4p_trace(G4P, "fps=%.1f\n", 1000.f / g4p_avgFramePeriod);
        }

        // we're done.
        g4p_onQuit();

        // Cleanup collision points system
        g4p_destroyCollisions();

        // Cleanup
        g4pi_destroy();

        return success;
    }
