#include <stdio.h>
#include <unistd.h>
#include <sys/times.h>
#include <X11/Xlib.h>
#include "gamemachine.h"
#include "_v4pi.h"

#define CLOCKS_PER_MSEC (CLOCKS_PER_SEC / 1000);

static Int32 SC_CLK_TCK = 0;

// get ticks
Int32 gmGetTicks() {
	
	static struct tms tmsBuffer;
	clock_t t = times(&tmsBuffer);
	if (!SC_CLK_TCK) SC_CLK_TCK = sysconf(_SC_CLK_TCK);

	return (t / SC_CLK_TCK) * 1000 + ((t % SC_CLK_TCK) * 1000) / SC_CLK_TCK;
}

// pause execution
void gmDelay(Int32 d) {
    usleep(d * 1000);
}

int gmPollEvents() {
  int rc = 0;
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
	    //v4pDisplayWidth = v4pDisplayContext->width  = report.xconfigure.width;
	    //v4pDisplayHeight = v4pDisplayContext->height = report.xconfigure.height;
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
	    rc = 1;
      break;
  	}
  return rc;
}
