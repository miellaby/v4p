#include <stdio.h>
#include "game_engine.h"
#include "v4p_ll.h"
#include "v4pi.h"
#include "g4pi.h"

// The game 4 pocket states holds basic up-to-date data
G4pState g4pState;

// framerate stuff
#define DEFAULT_FRAMERATE 60
#define MAX_PERIOD        (5 * 60000)
#define MAX_CATCHUP       5
int        g4pFramerate      = DEFAULT_FRAMERATE;
int        g4pAvgFramePeriod = 1000 / DEFAULT_FRAMERATE;
static int g4pPeriod         = 1000 / DEFAULT_FRAMERATE;  // private

// change the framerate
int        g4pSetFramerate(int new) {
  g4pFramerate = new;
  g4pPeriod    = (new > 0 ? 1000 / g4pFramerate : MAX_PERIOD);
  return (new);
}

// Game 4 Pocket main function
int g4pMain(int argc, char *argv[]) {
  Boolean rc = 0;
  Int32   beforeTime, afterTime, timeDiff, sleepTime, overSleepTime;
  Int32   lastTickTime = 0;
  Int32   deltaTime = 0;

  // reset game 4 pocket state
  g4pState.buttons[0] = 0;
  g4pState.key = 0;

  // Initialize
  g4piInit();

  // Init call-back
  if (g4pOnInit())
    return failure;

  lastTickTime = g4pGetTicks();
  while (!rc) {  // main game 4 pocket loop
    // Get current time and calculate delta since last tick
    beforeTime = g4pGetTicks();
    deltaTime = beforeTime - lastTickTime;
    lastTickTime = beforeTime;

    // process late ticks with period = max period (up to 5 late ticks max)
    int catchup = 0;
    while (!rc && deltaTime > g4pPeriod && catchup++ < MAX_CATCHUP) {
      rc |= g4pOnTick(g4pPeriod);
      deltaTime -= g4pPeriod;
    }

    // process current tick with remaining elapsed time
    if (!rc) {
      rc |= g4pOnTick(deltaTime);
      deltaTime = 0;
    }

    // poll user events
    rc |= g4pPollEvents();
    
    // render a frame
    rc |= g4pOnFrame();

    // calculate sleep time to maintain target frame rate
    afterTime = g4pGetTicks();
    timeDiff  = afterTime - beforeTime;
    sleepTime = (g4pPeriod - timeDiff);
    if (sleepTime <= 2) {
      sleepTime = 2;  // minimum sleep to prevent busy waiting
    }
    g4pDelay(sleepTime);
    g4pAvgFramePeriod = (3 * g4pAvgFramePeriod + timeDiff + sleepTime) / 4;
  }

  // we're done.
  g4pOnQuit();

  // Cleanup
  g4piDestroy();

  return success;
}
