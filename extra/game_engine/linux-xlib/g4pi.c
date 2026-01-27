#include <stdio.h>
#include <unistd.h>
#include <sys/times.h>
#include <X11/Xlib.h>
#include "game_engine.h"
#include "g4pi.h"
#include "_v4pi.h"

#define CLOCKS_PER_MSEC (CLOCKS_PER_SEC / 1000);

Int32 g4pGetTicks() {
  static struct tms buf;
  static int        clk_ticks = 0;
  if (!clk_ticks) {
    clk_ticks = sysconf(_SC_CLK_TCK);
#ifdef TESTU_GM_LINUX_SVGA
    printf("clk_ticks = %d\n", clk_ticks);
#endif
  }

  Int32 t = times(&buf) * 1000 / clk_ticks;
  return t;
}

// pause execution
void g4pDelay(Int32 d) {
  usleep(d * 1000);
}

// Initialize the game engine
void g4piInit() {
  // Xlib initialization is typically handled by the display system
  // This stub can be extended if needed for Xlib-specific initialization
}

// Cleanup the game engine
void g4piDestroy() {
  // Xlib cleanup is typically handled by the display system
  // This stub can be extended if needed for Xlib-specific cleanup
}

int g4pPollEvents() {
  int    rc = 0;
  XEvent report;
  // retrieve one event, returns if none
  if (!XCheckMaskEvent(currentDisplay, (~0), &report))
    return success;

  switch (report.type) {
    case Expose:
      // remove all Expose pending events
      while (XCheckTypedEvent(currentDisplay, Expose, &report))
        ;

      // one operate Expose events only when g4pFramerate == 0
      if (g4pFramerate == 0)
        rc |= g4pOnFrame();
      break;

    case ConfigureNotify:
      /*  Store new window width & height  */
      // v4pDisplayWidth = v4pDisplayContext->width  = report.xconfigure.width;
      // v4pDisplayHeight = v4pDisplayContext->height = report.xconfigure.height;
      break;

    case ButtonPress:
      g4pState.buttons[0] = 1;
      break;

    case ButtonRelease:
      g4pState.buttons[0] = 0;
      break;

    case MotionNotify:
      {
        int          root_x, root_y;
        int          pos_x, pos_y;
        Window       root, child;
        unsigned int keys_buttons;

        while (XCheckMaskEvent(currentDisplay, ButtonMotionMask, &report))
          ;
        if (!XQueryPointer(currentDisplay, report.xmotion.window,
                           &root, &child, &root_x, &root_y,
                           &pos_x, &pos_y, &keys_buttons))
          break;
        g4pState.xpen = pos_x;
        g4pState.ypen = pos_y;
        break;
      }
    case KeyPress:
      rc = 1;
      break;
  }
  return rc;
}
