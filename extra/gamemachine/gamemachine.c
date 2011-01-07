#include <stdio.h>
#include "gamemachine.h"
#include "v4p_ll.h"
#include "v4pi.h"
// gmi
Int32 gmGetTicks();
Int32 gmDelay();
int gmPollEvents();

// The machine states holds basic up-to-date data
GmState gmMachineState;

//framerate stuff
#define DEFAULT_FRAMERATE 30
#define MAX_PERIOD (5 * 60000)
#define MAX_SKIP 5 
int     gmFramerate = DEFAULT_FRAMERATE;
static  int gmPeriod = 1000 / DEFAULT_FRAMERATE; // private

// change the framerate
int gmSetFramerate(int new) {
  gmFramerate = new;
  gmPeriod = (new > 0 ? 1000 / gmFramerate : MAX_PERIOD);
	return (new);
}

// Machine main function
int gmMain(int argc, char* argv[])
{
    Boolean rc = 0;
    Int32 excess, beforeTime, overSleepTime, afterTime,
       timeDiff, sleepTime, repeat;

    // reset machine state
    gmMachineState.buttons[0] = 0;

    // Init call-back
    if (gmOnInit())
      return failure;

    afterTime = gmGetTicks();
    sleepTime = 0;
    excess = 0;
    while (!rc)  { //main machine loop
      // w/ clever hackery to handle properly performance drops
      beforeTime = gmGetTicks();
      overSleepTime = (beforeTime - afterTime) - sleepTime;

      // poll user events
      rc |= gmPollEvents();
      // process scene iteration
      rc |= gmOnIterate();
      // render a frame
      rc |= gmOnFrame();

      // maximize frame rates and detect performance drops
      afterTime = gmGetTicks();
      timeDiff = afterTime - beforeTime;
      sleepTime = (gmPeriod - timeDiff) - overSleepTime;
      if (sleepTime <= 0) {
        excess -= sleepTime;
        sleepTime = 2;
      }
      gmDelay(sleepTime);

      // when framerate is low, one repeats non-display steps
      repeat=MAX_SKIP; // max repeat
      while (repeat-- && excess > gmPeriod) {
        rc |= gmPollEvents();
        rc |= gmOnIterate();
        excess -= gmPeriod;
      }
      if (excess > gmPeriod) // max repeat reached
        excess = gmPeriod;
    }

    // we're done.    
    gmOnQuit();
    
    return success;
}
