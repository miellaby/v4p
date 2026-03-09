#include <vga.h>
#include "g4pi.h"

#ifdef TESTU_G4P_LINUX_SVGA
    #include <stdio.h>
#endif

// Initialize the game engine
void g4pi_init() {
    // SVGA initialization is typically handled by the display system
    // This stub can be extended if needed for SVGA-specific initialization
}

// Cleanup the game engine
void g4pi_destroy() {
    // SVGA cleanup is typically handled by the display system
    // This stub can be extended if needed for SVGA-specific cleanup
}

// poll user events
int g4pi_pollEvents() {
    static int firstRun = 1;

    int rc = 0;  // return code

    if (firstRun) {
        firstRun = 0;
    }

    char alph = vga_getkey();

    while (alph) {  // key loop
        switch (alph) {
            case 'q':
            case 'Q':
            case 27:
                {
                    rc = 1;
                    break;
                }
            default:  // TODO : Mouse not supported yet
                break;
        }  // switch
        alph = vga_getkey();
    }  // loop
    return rc;
}
