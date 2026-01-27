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
#define MAX_SKIP          5
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
  Int32   excess, beforeTime, overSleepTime, afterTime,
    timeDiff, sleepTime, repeat;

  // reset game 4 pocket state
  g4pState.buttons[0] = 0;
  g4pState.key = 0;

  // Initialize
  g4piInit();

  // Init call-back
  if (g4pOnInit())
    return failure;

  afterTime = g4pGetTicks();
  sleepTime = 0;
  excess    = 0;
  while (!rc) {  // main game 4 pocket loop
    // w/ clever hackery to handle properly performance drops
    beforeTime    = g4pGetTicks();
    overSleepTime = (beforeTime - afterTime) - sleepTime;

    // poll user events
    rc |= g4pPollEvents();
    // process scene iteration
    rc |= g4pOnIterate();
    // render a frame
    rc |= g4pOnFrame();

    // maximize frame rates and detect performance drops
    afterTime = g4pGetTicks();
    timeDiff  = afterTime - beforeTime;
    sleepTime = (g4pPeriod - timeDiff) - overSleepTime;
    if (sleepTime <= 0) {
      excess -= sleepTime;
      sleepTime = 2;
    }
    g4pAvgFramePeriod = (3 * g4pAvgFramePeriod + timeDiff + sleepTime + overSleepTime) / 4;
    g4pDelay(sleepTime);

    // when framerate is low, one repeats non-display steps
    repeat = MAX_SKIP;  // max repeat
    while (repeat-- && excess > g4pPeriod) {
      rc |= g4pPollEvents();
      rc |= g4pOnIterate();
      excess -= g4pPeriod;
    }
    if (excess > g4pPeriod)  // max repeat reached
      excess = g4pPeriod;
  }

  // we're done.
  g4pOnQuit();

  // Cleanup
  g4piDestroy();

  return success;
}
