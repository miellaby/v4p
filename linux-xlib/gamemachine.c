#include <stdio.h>
#include "gamemachine.h"
#include "v4p_ll.h"
#include "_v4pi.h"

#include <X11/Xlib.h>

GmState gmMachineState;

int gmMain(int argc, char* argv[])
{
    /* Miscellaneous X variables */
    XEvent        report;
    
    Boolean exit;
    
    gmMachineState.buttons[0] = 0;

    /* Game Machine */
    gmOnInit();
     
    while (!exit) {
	XNextEvent(currentDisplay, &report);

	switch ( report.type ) {

	case Expose:
        /* remove all Expose pending events */
        while (XCheckTypedEvent(currentDisplay, Expose, &report));
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
	    exit =1;
        break;
	}
    exit |= gmOnIterate();
    }
    
    gmOnQuit();
    
    return success;
}

