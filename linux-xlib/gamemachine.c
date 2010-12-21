#include <stdio.h>
#include "gamemachine.h"
#include "v4p_ll.h"
#include "_v4pi.h"

#include <X11/Xlib.h>

// The machine states holds basic up-to-date data
GmState gmMachineState;

#define DEFAULT_FRAMERATE 30
#define MAX_PERIOD (5 * 60000)
int     gmFramerate = DEFAULT_FRAMERATE;
static  int gmPeriod = 1000 / DEFAULT_FRAMERATE; // private

// change the framerate
int gmSetFramerate(int new) {
  gmFramerate = new;
  gmPeriod = (new > 0 ? 1000 / new : MAX_PERIOD);
	return (new);
}

static int gmPollEvents() {
  Boolean rc = false;
  XEvent report;
  // retrieve one event, returns if none
  if (!XCheckMaskEvent(currentDisplay, (~0), &report)) return success;

 	switch (report.type) {
   	case Expose:
      // remove all Expose pending events
      while (XCheckTypedEvent(currentDisplay, Expose, &report));

      // one operate Expose events only when gmFramerate == 0
      if (gmFramerate == 0)  rc |= gmOnFrame();
      break;
       
    case ConfigureNotify:
	    /*  Store new window width & height  */
	    v4pDisplayWidth = v4pDisplayContext->width  = report.xconfigure.width;
	    v4pDisplayHeight = v4pDisplayContext->height = report.xconfigure.height;
	    break;

  	case ButtonPress:
        gmMachineState.buttons[0] = 1;
        break;

    case ButtonRelease:
        gmMachineState.buttons[0] = 0;
        break;

    case MotionNotify: {
  	  int root_x, root_y;
      int pos_x, pos_y;
      Window root, child;
      unsigned int keys_buttons;
     
      while (XCheckMaskEvent(currentDisplay, ButtonMotionMask, &report));
      if (!XQueryPointer(currentDisplay, report.xmotion.window,
           &root, &child, &root_x, &root_y,
           &pos_x, &pos_y, &keys_buttons))
         break;
       gmMachineState.xpen = pos_x;
       gmMachineState.ypen = pos_y;
       break;
    }
    case KeyPress:
	    rc = true;
      break;
  	}
  return rc;
}


int gmMain(int argc, char* argv[])
{
    Boolean rc = false;
    Int32 excess, beforeTime, overSleepTime, afterTime,
       timeDiff, sleepTime;
    
    // Reset machine state    
    gmMachineState.buttons[0] = 0;

    // init call-back
    if (gmOnInit()) return failure;

    afterTime = SDL_GetTicks();
    sleepTime = 0;
    excess = 0;
    while (!rc)  { //main machine loop
      // w/ clever hackery to handle properly performance drops
      beforeTime = SDL_GetTicks();
      overSleepTime = (beforeTime - afterTime) - sleepTime;

      // poll user events
      rc |= gmPollEvents();
      // process scene iteration
      rc |= gmOnIterate();
      // render a frame
      rc |= gmOnFrame();

      // maximize frame rates and detect performance drops
      afterTime = SDL_GetTicks();
      timeDiff = afterTime - beforeTime;
      sleepTime = (gmPeriod - timeDiff) - overSleepTime;
      if (sleepTime <= 0) {
        excess -= sleepTime;
        sleepTime = 2;
      }
      SDL_Delay(sleepTime);

      // when framerate is low, one repeats non-display steps
      while (excess > gmPeriod) {
        rc |= gmPollEvents();
        rc |= gmOnIterate();
        excess -= gmPeriod;
      }
    }

    // we're done.
    gmOnQuit();
    
    return success;
}

